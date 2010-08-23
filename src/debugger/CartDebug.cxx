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
// Copyright (c) 1995-2010 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
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
    myRWPortAddress(0),
    myLabelLength(5)   // longest pre-defined label
{
  // Zero-page RAM is always present
  addRamArea(0x80, 128, 0, 0);

  // Add extended RAM
  for(RamAreaList::const_iterator i = areas.begin(); i != areas.end(); ++i)
    addRamArea(i->start, i->size, i->roffset, i->woffset);

  // Create an addresslist for each potential bank, and an extra one for ZP RAM
  for(int i = 0; i < myConsole.cartridge().bankCount()+1; ++i)
  {
    AddressList l;
    myEntryAddresses.push_back(l);
  }

  // We know the address for the startup bank right now
  myEntryAddresses[myConsole.cartridge().startBank()].push_back(myDebugger.dpeek(0xfffc));
  addLabel("START", myDebugger.dpeek(0xfffc));

  // Add system equates
  for(uInt16 addr = 0x00; addr <= 0x0F; ++addr)
    mySystemAddresses.insert(make_pair(ourTIAMnemonicR[addr], addr));
  for(uInt16 addr = 0x00; addr <= 0x3F; ++addr)
    mySystemAddresses.insert(make_pair(ourTIAMnemonicW[addr], addr));
  for(uInt16 addr = 0x280; addr <= 0x297; ++addr)
    mySystemAddresses.insert(make_pair(ourIOMnemonic[addr-0x280], addr));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartDebug::~CartDebug()
{
  myUserLabels.clear();
  myUserAddresses.clear();
  mySystemAddresses.clear();

  for(uInt32 i = 0; i < myEntryAddresses.size(); ++i)
    myEntryAddresses[i].clear();
  myEntryAddresses.clear();
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
    myState.ram.push_back(peek(myState.rport[i]));

  return myState;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::saveOldState()
{
  myOldState.ram.clear();
  for(uInt32 i = 0; i < myOldState.rport.size(); ++i)
    myOldState.ram.push_back(peek(myOldState.rport[i]));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::triggerReadFromWritePort(uInt16 addr)
{
  myRWPortAddress = addr;
  mySystem.setDirtyPage(addr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::readFromWritePort()
{
  uInt16 addr = myRWPortAddress;
  myRWPortAddress = 0;

  // A read from the write port occurs when the read is actually in the write
  // port address space AND the last access was actually a read (the latter
  // differentiates between reads that are normally part of a write cycle vs.
  // ones that are illegal)
  if(mySystem.m6502().lastReadAddress() &&
      (mySystem.getPageAccessType(addr) & System::PA_WRITE) == System::PA_WRITE)
    return addr;
  else
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
bool CartDebug::disassemble(const string& resolvedata, bool force)
{
  // Test current disassembly; don't re-disassemble if it hasn't changed
  // Also check if the current PC is in the current list
  bool bankChanged = myConsole.cartridge().bankChanged();
  uInt16 PC = myDebugger.cpuDebug().pc();
  int pcline = addressToLine(PC);
  bool pcfound = (pcline != -1) && ((uInt32)pcline < myDisassembly.list.size()) &&
                  (myDisassembly.list[pcline].disasm[0] != '.');
  bool pagedirty = (PC & 0x1000) ? mySystem.isPageDirty(0x1000, 0x1FFF) :
                                   mySystem.isPageDirty(0x80, 0xFF);

  bool changed = (force || bankChanged || !pcfound || pagedirty);
  if(changed)
  {
    // Are we disassembling from ROM or ZP RAM?
    AddressList& addresses = (PC & 0x1000) ? myEntryAddresses[getBank()] :
        myEntryAddresses[myEntryAddresses.size()-1];

    // If the offset has changed, all old addresses must be 'converted'
    // For example, if the list contains any $fxxx and the address space is now
    // $bxxx, it must be changed
    uInt16 offset = (PC - (PC % 0x1000));
    for(AddressList::iterator i = addresses.begin(); i != addresses.end(); ++i)
      *i = (*i & 0xFFF) + offset;

    // Only add addresses when absolutely necessary, to cut down on the
    // work that Distella has to do
    // Distella expects the addresses to be unique and in sorted order
    if(bankChanged || !pcfound)
    {
      AddressList::iterator i;
      for(i = addresses.begin(); i != addresses.end(); ++i)
      {
        if(PC < *i)
        {
          addresses.insert(i, PC);
          break;
        }
        else if(PC == *i)  // already present
          break;
      }
      // Otherwise, add the item at the end
      if(i == addresses.end())
        addresses.push_back(PC);
    }

    // Check whether to use the 'resolvedata' functionality from Distella
    if(resolvedata == "never")
      fillDisassemblyList(addresses, false, PC);
    else if(resolvedata == "always")
      fillDisassemblyList(addresses, true, PC);
    else  // 'auto'
    {
      // First try with resolvedata on, then turn off if PC isn't found
      if(!fillDisassemblyList(addresses, true, PC))
        fillDisassemblyList(addresses, false, PC);
    }
  }

  return changed;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartDebug::fillDisassemblyList(AddressList& addresses,
                                    bool resolvedata, uInt16 search)
{
  bool found = false;

  myDisassembly.list.clear();
  myDisassembly.fieldwidth = 10 + myLabelLength;
  uInt16 banksize =
    !BSPF_equalsIgnoreCase(myConsole.cartridge().name(), "Cartridge2K") ? 4 : 2;
  DiStella distella(*this, myDisassembly.list, addresses, banksize, resolvedata);

  // Parts of the disassembly will be accessed later in different ways
  // We place those parts in separate maps, to speed up access
  myAddrToLineList.clear();
  for(uInt32 i = 0; i < myDisassembly.list.size(); ++i)
  {
    const DisassemblyTag& tag = myDisassembly.list[i];

    // Only non-zero addresses are valid
    if(tag.address != 0)
    {
      // Create a mapping from addresses to line numbers
      myAddrToLineList.insert(make_pair(tag.address, i));

      // Did we find the search value?
      if(tag.address == search)
        found = true;
    }
  }
  return found;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::addressToLine(uInt16 address) const
{
  map<uInt16, int>::const_iterator iter = myAddrToLineList.find(address);
  return iter != myAddrToLineList.end() ? iter->second : -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::disassemble(uInt16 start, uInt16 lines) const
{
  Disassembly disasm;
  AddressList addresses;
  addresses.push_back(start);
  uInt16 banksize =
    !BSPF_equalsIgnoreCase(myConsole.cartridge().name(), "Cartridge2K") ? 4 : 2;
  DiStella distella(*this, disasm.list, addresses, banksize, false);

  // Fill the string with disassembled data
  start &= 0xFFF;
  ostringstream buffer;

  // First find the lines in the range, and determine the longest string
  uInt32 list_size = disasm.list.size();
  uInt32 begin = list_size, end = 0, length = 0;
  for(end = 0; end < list_size && lines > 0; ++end)
  {
    const CartDebug::DisassemblyTag& tag = disasm.list[end];
    if((tag.address & 0xfff) >= start)
    {
      if(begin == list_size) begin = end;
      length = BSPF_max(length, (uInt32)tag.disasm.length());

      --lines;
    }
  }

  // Now output the disassembly, using as little space as possible
  for(uInt32 i = begin; i < end; ++i)
  {
    const CartDebug::DisassemblyTag& tag = disasm.list[i];
    buffer << uppercase << hex << setw(4) << setfill('0') << tag.address
           << ":  " << tag.disasm << setw(length - tag.disasm.length() + 1)
           << setfill(' ') << " "
           << tag.ccount << "   " << tag.bytes << endl;
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
bool CartDebug::addLabel(const string& label, uInt16 address)
{
  // Only user-defined labels can be added or redefined
  switch(addressType(address))
  {
    case ADDR_TIA:
    case ADDR_RIOT:
      return false;
    default:
      removeLabel(label);
      myUserAddresses.insert(make_pair(label, address));
      myUserLabels.insert(make_pair(address, label));
      myLabelLength = BSPF_max(myLabelLength, (uInt16)label.size());
      mySystem.setDirtyPage(address);
      return true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartDebug::removeLabel(const string& label)
{
  // Only user-defined labels can be removed
  LabelToAddr::iterator iter = myUserAddresses.find(label);
  if(iter != myUserAddresses.end())
  {
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
    {
      uInt16 idx = (addr&0xff) - 0x80;
      if(idx < 24)
        return result = ourIOMnemonic[idx];
      break;
    }

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
  LabelToAddr::const_iterator iter;

  if((iter = mySystemAddresses.find(label)) != mySystemAddresses.end())
    return iter->second;
  else if((iter = myUserAddresses.find(label)) != myUserAddresses.end())
    return iter->second;
  else
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
void CartDebug::getCompletions(const char* in, StringList& completions) const
{
  // First scan system equates
  for(uInt16 addr = 0x00; addr <= 0x0F; ++addr)
    if(BSPF_strncasecmp(ourTIAMnemonicR[addr], in, strlen(in)) == 0)
      completions.push_back(ourTIAMnemonicR[addr]);
  for(uInt16 addr = 0x00; addr <= 0x3F; ++addr)
    if(BSPF_strncasecmp(ourTIAMnemonicW[addr], in, strlen(in)) == 0)
      completions.push_back(ourTIAMnemonicW[addr]);
  for(uInt16 addr = 0; addr <= 0x297-0x280; ++addr)
    if(BSPF_strncasecmp(ourIOMnemonic[addr], in, strlen(in)) == 0)
      completions.push_back(ourIOMnemonic[addr]);

  // Now scan user-defined labels
  LabelToAddr::const_iterator iter;
  for(iter = myUserAddresses.begin(); iter != myUserAddresses.end(); ++iter)
  {
    const char* l = iter->first.c_str();
    if(BSPF_strncasecmp(l, in, strlen(in)) == 0)
      completions.push_back(l);
  }
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
string CartDebug::extractLabel(const char *c) const
{
  string l = "";
  while(*c != ' ')
    l += *c++;

  return l;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int CartDebug::extractValue(const char *c) const
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
