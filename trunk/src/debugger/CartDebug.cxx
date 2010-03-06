//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#include "bspf.hxx"
#include "Array.hxx"
#include "System.hxx"
#include "DiStella.hxx"
#include "CpuDebug.hxx"
#include "CartDebug.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartDebug::CartDebug(Debugger& dbg, Console& console, const RamAreaList& areas)
  : DebuggerSystem(dbg, console),
    myRWPortAddress(0)
{
  // Zero-page RAM is always present
  addRamArea(0x80, 128, 0, 0);

  // Add extended RAM
  myRamAreas = areas;
  for(RamAreaList::const_iterator i = areas.begin(); i != areas.end(); ++i)
    addRamArea(i->start, i->size, i->roffset, i->woffset);

  // We need a start address for each potential bank
  myStartAddresses = new uInt16[myConsole.cartridge().bankCount()];
  for(int i = 0; i < myConsole.cartridge().bankCount(); ++i)
    myStartAddresses[i] = 0;

  // We know the address for the startup bank right now
  myStartAddresses[myConsole.cartridge().startBank()] = myDebugger.dpeek(0xfffc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartDebug::~CartDebug()
{
  delete[] myStartAddresses;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::addRamArea(uInt16 start, uInt16 size,
                           uInt16 roffset, uInt16 woffset)
{
  // First make sure this area isn't already present
  for(uInt32 i = 0; i < myState.rport.size(); ++i)
    if(myState.rport[i] == start + roffset ||
       myState.wport[i] == start + woffset)
      return;

  // Otherwise, add a new area
  for(uInt32 i = 0; i < size; ++i)
  {
    myState.rport.push_back(i + start + roffset);
    myState.wport.push_back(i + start + woffset);

    myOldState.rport.push_back(i + start + roffset);
    myOldState.wport.push_back(i + start + woffset);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const DebuggerState& CartDebug::getState()
{
  myState.ram.clear();
  for(uInt32 i = 0; i < myState.rport.size(); ++i)
    myState.ram.push_back(read(myState.rport[i]));

  return myState;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::saveOldState()
{
  myOldState.ram.clear();
  for(uInt32 i = 0; i < myOldState.rport.size(); ++i)
    myOldState.ram.push_back(read(myOldState.rport[i]));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt8 CartDebug::read(uInt16 addr)
{
  return mySystem.peek(addr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::write(uInt16 addr, uInt8 value)
{
  mySystem.poke(addr, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::readFromWritePort()
{
  uInt16 peekAddress = myRWPortAddress;
  myRWPortAddress = 0;

  // A read from the write port occurs when the read is actually in the write
  // port address space AND the last access was actually a read (the latter
  // differentiates between reads that are normally part of a write cycle vs.
  // ones that are illegal)
  if(mySystem.m6502().lastReadAddress() && peekAddress & 0x1000)
  {
    uInt16 addr = peekAddress & 0x0FFF;
    for(RamAreaList::const_iterator i = myRamAreas.begin(); i != myRamAreas.end(); ++i)
    {
      uInt16 start = (i->start + i->woffset) & 0x0FFF;
      uInt16 end = (i->start + i->woffset + i->size) & 0x0FFF;
      if(addr >= start && addr < end)
        return peekAddress;
    }
  }
  return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::toString()
{
  string result;
  char buf[128];
  uInt32 bytesPerLine;

  switch(myDebugger.parser().base())
  {
    case kBASE_16:
    case kBASE_10:
      bytesPerLine = 0x10;
      break;

    case kBASE_2:
      bytesPerLine = 0x04;
      break;

    case kBASE_DEFAULT:
    default:
      return DebuggerParser::red("invalid base, this is a BUG");
  }

  const CartState& state    = (CartState&) getState();
  const CartState& oldstate = (CartState&) getOldState();

  uInt32 curraddr = 0, bytesSoFar = 0;
  for(uInt32 i = 0; i < state.ram.size(); i += bytesPerLine, bytesSoFar += bytesPerLine)
  {
    // We detect different 'pages' of RAM when the addresses jump by
    // more than the number of bytes on the previous line, or when 256
    // bytes have been previously output
    if(state.rport[i] - curraddr > bytesPerLine || bytesSoFar >= 256)
    {
      sprintf(buf, "%04x: (rport = %04x, wport = %04x)\n",
              state.rport[i], state.rport[i], state.wport[i]);
      buf[2] = buf[3] = 'x';
      result += DebuggerParser::red(buf);
      bytesSoFar = 0;
    }
    curraddr = state.rport[i];
    sprintf(buf, "%.2x: ", curraddr & 0x00ff);
    result += buf;

    for(uInt8 j = 0; j < bytesPerLine; ++j)
    {
      result += myDebugger.invIfChanged(state.ram[i+j], oldstate.ram[i+j]);
      result += " ";

      if(j == 0x07) result += " ";
    }
    result += "\n";
  }

  return result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartDebug::disassemble(bool autocode)
{
  // Test current disassembly; don't re-disassemble if it hasn't changed
  // ...
  bool changed = myConsole.cartridge().bankChanged();
  if(changed)
  {
    myDisassembly.clear();
    myAddrToLineList.clear();

    // We only use the reset vector when we're actually in the startup bank
    // Otherwise, we look at previous accesses to this bank to begin
    // If no previous address exists, use the current program counter
    uInt16 start = myStartAddresses[getBank()];

cerr << "start addresses: ";
for(int i = 0; i < myConsole.cartridge().bankCount(); ++i) cerr << " " << setw(4) << hex << myStartAddresses[i];
cerr << endl;
cerr << "current bank = " << getBank() << ", start bank = " << myConsole.cartridge().startBank() << endl
    << "reset = " << hex << 0xfffc << ", pc = " << hex << myDebugger.cpuDebug().pc() << endl
    << "start = " << hex << start << endl;
    if(start == 0)
      start = myStartAddresses[getBank()] = myDebugger.cpuDebug().pc();
cerr << "actual start = " << hex << start << endl << endl;

    // For now, DiStella can't handle address space below 0x1000
    if(!(start & 0x1000))
      return true;

autocode = false;
    DiStella distella(myDisassembly, start, autocode);

    // Parts of the disassembly will be accessed later in different ways
    // We place those parts in separate maps, to speed up access
    for(uInt32 i = 0; i < myDisassembly.size(); ++i)
    {
      const DisassemblyTag& tag = myDisassembly[i];

      // Create a mapping from addresses to line numbers
      if(tag.address != 0)
        myAddrToLineList.insert(make_pair(tag.address, i));

      // TODO - look at list, extract address to label mappings
      //        we need these for label support in the UI and promptwidget
    }
  }

  return changed;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::addressToLine(uInt16 address) const
{
  map<uInt16, int>::const_iterator iter = myAddrToLineList.find(address);
  return iter != myAddrToLineList.end() ? iter->second : 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::disassemble(uInt16 start, uInt16 lines) const
{
  if(!(start & 0x1000))
    return DebuggerParser::red("Disassembly below 0x1000 not yet supported");

  DisassemblyList list;
  DiStella distella(list, start, false);

  // Fill the string with disassembled data
  start &= 0xFFF;
  ostringstream buffer;
  for(uInt32 i = 0; i < list.size() && lines > 0; ++i, --lines)
  {
    const CartDebug::DisassemblyTag& tag = list[i];
    if((tag.address & 0xfff) >= start)
    {
      buffer << uppercase << hex << setw(4) << setfill('0') << tag.address
             << ":  " << tag.disasm << "  " << tag.bytes << endl;
    }
  }

  return buffer.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::getBank()
{
  return myConsole.cartridge().bank();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::bankCount()
{
  return myConsole.cartridge().bankCount();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::getCartType()
{
  return myConsole.cartridge().name();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::addLabel(const string& label, uInt16 address)
{
  // Only user-defined labels can be added
  switch(addressType(address))
  {
    case ADDR_TIA:
    case ADDR_RIOT:
      return;
    default:
cerr << "addLabel: label = " << label << ", address = " << hex << address << endl;
      removeLabel(label);
      myUserAddresses.insert(make_pair(label, address));
      myUserLabels.insert(make_pair(address, label));
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartDebug::removeLabel(const string& label)
{
  // Only user-defined labels can be removed
  LabelToAddr::iterator iter = myUserAddresses.find(label);
  if(iter != myUserAddresses.end())
  {
cerr << "removeLabel: label = " << label << endl;
    // Erase the label
    myUserAddresses.erase(iter);

    // And also erase the address assigned to it
    AddrToLabel::iterator iter2 = myUserLabels.find(iter->second);
    if(iter2 != myUserLabels.end())
      myUserLabels.erase(iter2);

    return true;
  }
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string& CartDebug::getLabel(uInt16 addr, bool isRead, int places) const
{
  static string result;

  switch(addressType(addr))
  {
    case ADDR_TIA:
      return result =
        (isRead ? ourTIAMnemonicR[addr&0x0f] : ourTIAMnemonicW[addr&0x3f]);

    case ADDR_RIOT:
      return result = ourIOMnemonic[(addr&0xf00)-280];

    case ADDR_RAM:
    case ADDR_ROM:
    {
      // These addresses can never be in the system labels list
      AddrToLabel::const_iterator iter;
      if((iter = myUserLabels.find(addr)) != myUserLabels.end())
        return iter->second;
      break;
    }
  }

  if(places > -1)
  {
    ostringstream buf;
    buf << "$" << setw(places) << hex << addr;
    return result = buf.str();
  }

  return EmptyString;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::getAddress(const string& label) const
{
/* FIXME
  LabelToAddr::const_iterator iter;

  if((iter = mySystemAddresses.find(label)) != mySystemAddresses.end())
    return iter->second.address;
  else if((iter = myUserAddresses.find(label)) != myUserAddresses.end())
    return iter->second.address;
  else
    return -1;
*/
return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::loadSymbolFile(const string& f)
{
  string file = f;
  string::size_type spos;
  if( (spos = file.find_last_of('.')) != string::npos )
    file.replace(spos, file.size(), ".sym");
  else
    file += ".sym";

cerr << "loadSymbolFile: " << file << endl;

  int pos = 0, lines = 0, curVal;
  string curLabel;
  char line[1024];

  ifstream in(file.c_str());
  if(!in.is_open())
    return "Unable to read symbols from " + file;

  myUserAddresses.clear();
  myUserLabels.clear();

  while( !in.eof() )
  {
    curVal = 0;
    curLabel = "";

    int got = in.get();

    if(got == -1 || got == '\r' || got == '\n' || pos == 1023) {
      line[pos] = '\0';
      pos = 0;

      if(strlen(line) > 0 && line[0] != '-')
      {
        curLabel = extractLabel(line);
        if((curVal = extractValue(line)) < 0)
          return "invalid symbol file";
  
        addLabel(curLabel, curVal);
  
        lines++;
      }
    }
    else
    {
      line[pos++] = got;
    }
  }
  in.close();
  return "loaded " + file + " OK";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::countCompletions(const char *in)
{
/* FIXME
  myCompletions = myCompPrefix = "";
  return countCompletions(in, mySystemAddresses) +
         countCompletions(in, myUserAddresses);
*/
return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::countCompletions(const char *in, LabelToAddr& addresses)
{
  int count = 0;

  LabelToAddr::iterator iter;
  for(iter = addresses.begin(); iter != addresses.end(); iter++)
  {
    const char *l = iter->first.c_str();

    if(BSPF_strncasecmp(l, in, strlen(in)) == 0)
    {
      if(myCompPrefix == "")
        myCompPrefix += l;
      else
      {
        int nonMatch = 0;
        const char *c = myCompPrefix.c_str();
        while(*c != '\0' && tolower(*c) == tolower(l[nonMatch]))
        {
          c++;
          nonMatch++;
        }
        myCompPrefix.erase(nonMatch, myCompPrefix.length());
      }

      if(count++) myCompletions += "  ";
        myCompletions += l;
    }
  }

  return count;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartDebug::AddrType CartDebug::addressType(uInt16 addr) const
{
  // Determine the type of address to access the correct list
  // These addresses were based on (and checked against) Kroko's 2600 memory
  // map, found at http://www.qotile.net/minidig/docs/2600_mem_map.txt
  AddrType type = ADDR_ROM;
  if(addr % 0x2000 < 0x1000)
  {
    uInt16 z = addr & 0x00ff;
    if(z < 0x80)
      type = ADDR_TIA;
    else
    {
      switch(addr & 0x0f00)
      {
        case 0x000:
        case 0x100:
        case 0x400:
        case 0x500:
        case 0x800:
        case 0x900:
        case 0xc00:
        case 0xd00:
          type = ADDR_RAM;
          break;
        case 0x200:
        case 0x300:
        case 0x600:
        case 0x700:
        case 0xa00:
        case 0xb00:
        case 0xe00:
        case 0xf00:
          type = ADDR_RIOT;
          break;
      }
    }
  }
  return type;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::extractLabel(char *c) const
{
  string l = "";
  while(*c != ' ')
    l += *c++;

  return l;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::extractValue(char *c) const
{
  while(*c != ' ')
  {
    if(*c == '\0')
      return -1;
    c++;
  }

  while(*c == ' ')
  {
    if(*c == '\0')
      return -1;
    c++;
  }

  int ret = 0;
  for(int i=0; i<4; i++)
  {
    if(*c >= '0' && *c <= '9')
      ret = (ret << 4) + (*c) - '0';
    else if(*c >= 'a' && *c <= 'f')
      ret = (ret << 4) + (*c) - 'a' + 10;
    else
      return -1;
    c++;
  }
  return ret;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CartDebug::ourTIAMnemonicR[16] = {
  "CXM0P", "CXM1P", "CXP0FB", "CXP1FB", "CXM0FB", "CXM1FB", "CXBLPF", "CXPPMM",
  "INPT0", "INPT1", "INPT2", "INPT3", "INPT4", "INPT5", "$0E", "$0F"
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CartDebug::ourTIAMnemonicW[64] = {
  "VSYNC", "VBLANK", "WSYNC", "RSYNC", "NUSIZ0", "NUSIZ1", "COLUP0", "COLUP1",
  "COLUPF", "COLUBK", "CTRLPF", "REFP0", "REFP1", "PF0", "PF1", "PF2",
  "RESP0", "RESP1", "RESM0", "RESM1", "RESBL", "AUDC0", "AUDC1", "AUDF0",
  "AUDF1", "AUDV0", "AUDV1", "GRP0", "GRP1", "ENAM0", "ENAM1", "ENABL",
  "HMP0", "HMP1", "HMM0", "HMM1", "HMBL", "VDELP0", "VDELP1", "VDELBL",
  "RESMP0", "RESMP1", "HMOVE", "HMCLR", "CXCLR", "$2D", "$2E", "$2F",
  "$30", "$31", "$32", "$33", "$34", "$35", "$36", "$37",
  "$38", "$39", "$3A", "$3B", "$3C", "$3D", "$3E", "$3F"
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CartDebug::ourIOMnemonic[24] = {
  "SWCHA", "SWACNT", "SWCHB", "SWBCNT", "INTIM", "TIMINT", "$0286", "$0287",
  "$0288", "$0289", "$028A", "$028B", "$028C", "$028D", "$028E", "$028F",
  "$0290", "$0291", "$0292", "$0293", "TIM1T", "TIM8T", "TIM64T", "T1024T"
};
