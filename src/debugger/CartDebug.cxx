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
// Copyright (c) 1995-2013 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#include <time.h>

#include "bspf.hxx"
#include "Array.hxx"
#include "System.hxx"
#include "FSNode.hxx"
#include "DiStella.hxx"
#include "Debugger.hxx"
#include "CpuDebug.hxx"
#include "OSystem.hxx"
#include "Settings.hxx"
#include "Version.hxx"
#include "CartDebug.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartDebug::CartDebug(Debugger& dbg, Console& console, const OSystem& osystem)
  : DebuggerSystem(dbg, console),
    myOSystem(osystem),
    myRWPortAddress(0),
    myLabelLength(8)   // longest pre-defined label
{
  // Zero-page RAM is always present
  addRamArea(0x80, 128, 0, 0);

  // Add extended RAM
  const RamAreaList& areas = console.cartridge().ramAreas();
  for(RamAreaList::const_iterator i = areas.begin(); i != areas.end(); ++i)
    addRamArea(i->start, i->size, i->roffset, i->woffset);

  // Create bank information for each potential bank, and an extra one for ZP RAM
  // Banksizes greater than 4096 indicate multi-bank ROMs, but we handle only
  // 4K pieces at a time
  // Banksizes less than 4K use the actual value
  int banksize = 4096;
  myConsole.cartridge().getImage(banksize);
  banksize = BSPF_min(banksize, 4096);
  BankInfo info;
  for(int i = 0; i < myConsole.cartridge().bankCount(); ++i)
  {
    info.size = banksize;
    myBankInfo.push_back(info);
  }
  info.size = 128;  // ZP RAM
  myBankInfo.push_back(info);

  // We know the address for the startup bank right now
  myBankInfo[myConsole.cartridge().startBank()].addressList.push_back(myDebugger.dpeek(0xfffc));
  addLabel("START", myDebugger.dpeek(0xfffc));

  // Add system equates
  for(uInt16 addr = 0x00; addr <= 0x3F; ++addr)
  {
    mySystemAddresses.insert(make_pair(ourTIAMnemonicR[addr], addr));
    myReserved.TIARead[addr] = false;
  }
  for(uInt16 addr = 0x00; addr <= 0x7F; ++addr)
  {
    mySystemAddresses.insert(make_pair(ourTIAMnemonicW[addr], addr));
    myReserved.TIAWrite[addr] = false;
  }
  for(uInt16 addr = 0x280; addr <= 0x297; ++addr)
  {
    mySystemAddresses.insert(make_pair(ourIOMnemonic[addr-0x280], addr));
    myReserved.IOReadWrite[addr-0x280] = false;
  }

  myReserved.Label.clear();

  // Add settings for Distella
  DiStella::settings.gfx_format =
    myOSystem.settings().getInt("dis.gfxformat") == 16 ? kBASE_16 : kBASE_2;
  DiStella::settings.show_addresses =
    myOSystem.settings().getBool("dis.showaddr");
  DiStella::settings.aflag = false; // Not currently configurable
  DiStella::settings.fflag = true;  // Not currently configurable
  DiStella::settings.rflag = myOSystem.settings().getBool("dis.relocate");
  DiStella::settings.bwidth = 9;  // TODO - configure based on window size
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartDebug::~CartDebug()
{
  myUserLabels.clear();
  myUserAddresses.clear();
  mySystemAddresses.clear();

  for(uInt32 i = 0; i < myBankInfo.size(); ++i)
  {
    myBankInfo[i].addressList.clear();
    myBankInfo[i].directiveList.clear();
  }
  myBankInfo.clear();
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
  ostringstream buf;
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
      char port[37];
      BSPF_snprintf(port, 36, "%04x: (rport = %04x, wport = %04x)\n",
              state.rport[i], state.rport[i], state.wport[i]);
      port[2] = port[3] = 'x';
      buf << DebuggerParser::red(port);
      bytesSoFar = 0;
    }
    curraddr = state.rport[i];
    buf << HEX2 << (curraddr & 0x00ff) << ": ";

    for(uInt8 j = 0; j < bytesPerLine; ++j)
    {
      buf << myDebugger.invIfChanged(state.ram[i+j], oldstate.ram[i+j]) << " ";

      if(j == 0x07) buf << " ";
    }
    buf << endl;
  }

  return buf.str();
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
    BankInfo& info = (PC & 0x1000) ? myBankInfo[getBank()] :
        myBankInfo[myBankInfo.size()-1];

    // If the offset has changed, all old addresses must be 'converted'
    // For example, if the list contains any $fxxx and the address space is now
    // $bxxx, it must be changed
    uInt16 offset = (PC - (PC % 0x1000));
    AddressList& addresses = info.addressList;
    for(list<uInt16>::iterator i = addresses.begin(); i != addresses.end(); ++i)
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
      fillDisassemblyList(info, false, PC);
    else if(resolvedata == "always")
      fillDisassemblyList(info, true, PC);
    else  // 'auto'
    {
      // First try with resolvedata on, then turn off if PC isn't found
      if(!fillDisassemblyList(info, true, PC))
        fillDisassemblyList(info, false, PC);
    }
  }

  return changed;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartDebug::fillDisassemblyList(BankInfo& info, bool resolvedata, uInt16 search)
{
  myDisassembly.list.clear();
  myDisassembly.list.reserve(2048);
  myDisassembly.fieldwidth = 10 + myLabelLength;
  DiStella distella(*this, myDisassembly.list, info, DiStella::settings,
                    myDisLabels, myDisDirectives, myReserved, resolvedata);

  // Parts of the disassembly will be accessed later in different ways
  // We place those parts in separate maps, to speed up access
  bool found = false;
  myAddrToLineList.clear();
  for(uInt32 i = 0; i < myDisassembly.list.size(); ++i)
  {
    const DisassemblyTag& tag = myDisassembly.list[i];

    // Addresses marked as 'ROW' normally won't have an address
    if(tag.address)
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
  BankInfo info;
  info.addressList.push_back(start);
  DiStella distella(*this, disasm.list, info, DiStella::settings,
                    (uInt8*)myDisLabels, (uInt8*)myDisDirectives,
                    (ReservedEquates&)myReserved, false);

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
      if(tag.type != CartDebug::ROW)
        length = BSPF_max(length, (uInt32)tag.disasm.length());

      --lines;
    }
  }

  // Now output the disassembly, using as little space as possible
  for(uInt32 i = begin; i < end; ++i)
  {
    const CartDebug::DisassemblyTag& tag = disasm.list[i];
    if(tag.type == CartDebug::NONE)
      continue;
    else if(tag.address)
      buffer << uppercase << hex << setw(4) << setfill('0') << tag.address
           << ":  ";
    else
      buffer << "       ";

    buffer << tag.disasm << setw(length - tag.disasm.length() + 1)
           << setfill(' ') << " "
           << tag.ccount << "   " << tag.bytes << endl;
  }

  return buffer.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartDebug::addDirective(CartDebug::DisasmType type,
                             uInt16 start, uInt16 end, int bank)
{
#define PRINT_TAG(tag) \
  disasmTypeAsString(cerr, tag.type); \
  cerr << ": start = " << tag.start << ", end = " << tag.end << endl;

#define PRINT_LIST(header) \
  cerr << header << endl; \
  for(DirectiveList::const_iterator d = list.begin(); d != list.end(); ++d) { \
    PRINT_TAG((*d)); } \
  cerr << endl;

  if(end < start || start == 0 || end == 0)
    return false;

  if(bank < 0)  // Do we want the current bank or ZP RAM?
    bank = (myDebugger.cpuDebug().pc() & 0x1000) ? getBank() : myBankInfo.size()-1;

  bank = BSPF_min(bank, bankCount());
  BankInfo& info = myBankInfo[bank];
  DirectiveList& list = info.directiveList;

  DirectiveTag tag;
  tag.type = type;
  tag.start = start;
  tag.end = end;

  DirectiveList::iterator i;

  // If the same directive and range is added, consider it a removal instead
  for(i = list.begin(); i != list.end(); ++i)
  {
    if(i->type == tag.type && i->start == tag.start && i->end == tag.end)
    {
      list.erase(i);
      return false;
    }
  }

  // Otherwise, scan the list and make space for a 'smart' merge
  // Note that there are 4 possibilities:
  //  1: a range is completely inside the new range
  //  2: a range is completely outside the new range
  //  3: a range overlaps at the beginning of the new range
  //  4: a range overlaps at the end of the new range
  for(i = list.begin(); i != list.end(); ++i)
  {
    // Case 1: remove range that is completely inside new range
    if(tag.start <= i->start && tag.end >= i->end)
    {
      i = list.erase(i);
    }
    // Case 2: split the old range
    else if(tag.start >= i->start && tag.end <= i->end)
    {
      // Only split when necessary
      if(tag.type == i->type)
        return true;  // node is fine as-is

      // Create new endpoint
      DirectiveTag tag2;
      tag2.type = i->type;
      tag2.start = tag.end + 1;
      tag2.end = i->end;

      // Modify startpoint
      i->end = tag.start - 1;

      // Insert new endpoint
      i++;
      list.insert(i, tag2);
      break;  // no need to go further; this is the insertion point
    }
    // Case 3: truncate end of old range
    else if(tag.start >= i->start && tag.start <= i->end)
    {
      i->end = tag.start - 1;
    }
    // Case 4: truncate start of old range
    else if(tag.end >= i->start && tag.end <= i->end)
    {
      i->start = tag.end + 1;
    }
  }

  // We now know that the new range can be inserted without overlap
  // Where possible, consecutive ranges should be merged rather than
  // new nodes created
  for(i = list.begin(); i != list.end(); ++i)
  {
    if(tag.end < i->start)  // node should be inserted *before* this one
    {
      bool createNode = true;

      // Is the new range ending consecutive with the old range beginning?
      // If so, a merge will suffice
      if(i->type == tag.type && tag.end + 1 == i->start)
      {
        i->start = tag.start;
        createNode = false;  // a merge was done, so a new node isn't needed
      }

      // Can we also merge with the previous range (if any)?
      if(i != list.begin())
      {
        DirectiveList::iterator p = i;
        --p;
        if(p->type == tag.type && p->end + 1 == tag.start)
        {
          if(createNode)  // a merge with right-hand range didn't previously occur
          {
            p->end = tag.end;
            createNode = false;  // a merge was done, so a new node isn't needed
          }
          else  // merge all three ranges
          {
            i->start = p->start;
            i = list.erase(p);
            createNode = false;  // a merge was done, so a new node isn't needed
          }
        }
      }

      // Create the node only when necessary
      if(createNode)
        i = list.insert(i, tag);

      break;
    }
  }
  // Otherwise, add the tag at the end
  if(i == list.end())
    list.push_back(tag);

  return true;
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
        (isRead ? ourTIAMnemonicR[addr&0x3f] : ourTIAMnemonicW[addr&0x7f]);

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
string CartDebug::loadSymbolFile(string file)
{
  if(file == "")
    file = myOSystem.romFile();

  string::size_type spos;
  if( (spos = file.find_last_of('.')) != string::npos )
    file.replace(spos, file.size(), ".sym");
  else
    file += ".sym";

  FilesystemNode node(file);
  if(node.exists() && node.isFile())
  {
    ifstream in(node.getPath().c_str());
    if(!in.is_open())
      return DebuggerParser::red("symbol file '" + node.getShortPath() + "' not found");

    myUserAddresses.clear();
    myUserLabels.clear();

    while(!in.eof())
    {
      string label;
      int value = -1;

      getline(in, label);
      stringstream buf;
      buf << label;
      buf >> label >> hex >> value;

      if(label.length() > 0 && label[0] != '-' && value >= 0)
        addLabel(label, value);
    }
    in.close();
    return "loaded " + node.getShortPath() + " OK";
  }
  return DebuggerParser::red("symbol file '" + node.getShortPath() + "' not found");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::loadConfigFile(string file)
{
  FilesystemNode node(file);

  if(file == "")
  {
    // There are three possible locations for loading config files
    //   (in order of decreasing relevance):
    // 1) ROM dir based on properties entry name
    // 2) ROM dir based on actual ROM name
    // 3) CFG dir based on properties entry name

    const string& propsname =
      myConsole.properties().get(Cartridge_Name) + ".cfg";

    // Case 1
    FilesystemNode case1(FilesystemNode(myOSystem.romFile()).getParent().getPath() +
                         propsname);
    if(case1.exists())
    {
      node = case1;
    }
    else
    {
      file = myOSystem.romFile();
      string::size_type spos;
      if((spos = file.find_last_of('.')) != string::npos )
        file.replace(spos, file.size(), ".cfg");
      else
        file += ".cfg";
      FilesystemNode case2(file);
      if(case2.exists())
      {
        node = case2;
      }
      else  // Use global config file based on properties cart name
      {
        FilesystemNode case3(myOSystem.cfgDir() + propsname);
        if(case3.exists())
          node = case3;
      }
    }
  }

  if(node.exists() && node.isFile())
  {
    ifstream in(node.getPath().c_str());
    if(!in.is_open())
      return "Unable to load directives from " + node.getPath();

    // Erase all previous directives
    for(Common::Array<BankInfo>::iterator bi = myBankInfo.begin();
        bi != myBankInfo.end(); ++bi)
    {
      bi->directiveList.clear();
    }

    int currentbank = 0;
    while(!in.eof())
    {
      // Skip leading space
      int c = in.peek();
      while(c == ' ' && c == '\t')
      {
        in.get();
        c = in.peek();
      }

      string line;
      c = in.peek();
      if(c == '/')  // Comment, swallow line and continue
      {
        getline(in, line);
        continue;
      }
      else if(c == '[')
      {
        in.get();
        getline(in, line, ']');
        stringstream buf(line);
        buf >> currentbank;
      }
      else  // Should be commands from this point on
      {
        getline(in, line);
        stringstream buf;
        buf << line;

        string directive;
        uInt16 start = 0, end = 0;
        buf >> directive;
        if(BSPF_startsWithIgnoreCase(directive, "ORG"))
        {
          // TODO - figure out what to do with this
          buf >> hex >> start;
        }
        else if(BSPF_startsWithIgnoreCase(directive, "CODE"))
        {
          buf >> hex >> start >> hex >> end;
          addDirective(CartDebug::CODE, start, end, currentbank);
        }
        else if(BSPF_startsWithIgnoreCase(directive, "GFX"))
        {
          buf >> hex >> start >> hex >> end;
          addDirective(CartDebug::GFX, start, end, currentbank);
        }
        else if(BSPF_startsWithIgnoreCase(directive, "PGFX"))
        {
          buf >> hex >> start >> hex >> end;
          addDirective(CartDebug::PGFX, start, end, currentbank);
        }
        else if(BSPF_startsWithIgnoreCase(directive, "DATA"))
        {
          buf >> hex >> start >> hex >> end;
          addDirective(CartDebug::DATA, start, end, currentbank);
        }
        else if(BSPF_startsWithIgnoreCase(directive, "ROW"))
        {
          buf >> hex >> start;
          buf >> hex >> end;
          addDirective(CartDebug::ROW, start, end, currentbank);
        }
      }
    }
    in.close();

    return "loaded " + node.getShortPath() + " OK";
  }
  else
    return DebuggerParser::red("config file not found");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::saveConfigFile(string file)
{
  FilesystemNode node(file);

  const string& name = myConsole.properties().get(Cartridge_Name);
  const string& md5 = myConsole.properties().get(Cartridge_MD5);

  if(file == "")
  {
    // There are two possible locations for saving config files
    //   (in order of decreasing relevance):
    // 1) ROM dir based on properties entry name
    // 2) ROM dir based on actual ROM name
    //
    // In either case, we're using the properties entry, since even ROMs that
    // don't have a proper entry have a temporary one inserted by OSystem
    node = FilesystemNode(FilesystemNode(
        myOSystem.romFile()).getParent().getPath() + name + ".cfg");
  }

  if(node.isFile())
  {
    ofstream out(node.getPath().c_str());
    if(!out.is_open())
      return "Unable to save directives to " + node.getPath();

    // Store all bank information
    out << "//Stella.pro: \"" << name << "\"" << endl
        << "//MD5: " << md5 << endl
        << endl;
    for(uInt32 b = 0; b < myConsole.cartridge().bankCount(); ++b)
    {
      out << "[" << b << "]" << endl;
      getBankDirectives(out, myBankInfo[b]);
    }
    out.close();

    return "saved " + node.getShortPath() + " OK";
  }
  else
    return DebuggerParser::red("config file not found");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::saveDisassembly(string file)
{
#define ALIGN(x) setfill(' ') << left << setw(x)

  if(myConsole.cartridge().bankCount() > 1)
    return DebuggerParser::red("multi-bank ROM not yet supported");

  // Output stream for disassembly
  ostringstream buf;

  // Some boilerplate, similar to what DiStella adds
  time_t currtime;
  time(&currtime);
  buf << "; Disassembly of " << file << "\n"
      << "; Disassembled " << ctime(&currtime)
      << "; Using Stella " << STELLA_VERSION << "\n;\n"
      << "; Settings used: TODO - add args\n;\n"
      << "; Legend: * = CODE not yet run (preliminary code)\n"
      << ";         D = DATA directive (referenced in some way)\n"
      << ";         G = GFX directive, shown as '#' (stored in player, missile, ball)\n"
      << ";         P = PGFX directive, shown as '*' (stored in playfield)\n"
      << "\n      processor 6502\n\n";

  for(uInt16 addr = 0x00; addr <= 0x3F; ++addr)
    if(myReserved.TIARead[addr] && ourTIAMnemonicR[addr][0] != '$')
      buf << ALIGN(10) << ourTIAMnemonicR[addr] << " =  $"
          << HEX2 << right << addr << " ; (R)\n";

  for(uInt16 addr = 0x00; addr <= 0x7F; ++addr)
    if(myReserved.TIAWrite[addr] && ourTIAMnemonicW[addr][0] != '$')
      buf << ALIGN(10) << ourTIAMnemonicW[addr] << " =  $"
          << HEX2 << right << addr << " ; (W)\n";

  for(uInt16 addr = 0x00; addr <= 0x17; ++addr)
    if(myReserved.IOReadWrite[addr] && ourIOMnemonic[addr][0] != '$')
      buf << ALIGN(10) << ourIOMnemonic[addr] << " =  $"
          << HEX4 << right << (addr+0x280) << "\n";

  AddrToLabel::const_iterator iter;
  for(iter = myReserved.Label.begin(); iter != myReserved.Label.end(); ++iter)
      buf << ALIGN(10) << iter->second << " =  $" << iter->first << "\n";

  buf << "\n";

  // Use specific settings for disassembly output
  // This will most likely differ from what you see in the debugger
  DiStella::Settings settings;
  settings.gfx_format = DiStella::settings.gfx_format;
  settings.show_addresses = false;
  settings.aflag = false; // Otherwise DASM gets confused
  settings.fflag = DiStella::settings.fflag;
  settings.rflag = DiStella::settings.rflag;
  settings.bwidth = 17;  // default from Distella

  Disassembly disasm;
  disasm.list.reserve(2048);
  disasm.fieldwidth = 10 + myLabelLength;
  for(int bank = 0; bank < myConsole.cartridge().bankCount(); ++bank)
  {
    BankInfo& info = myBankInfo[bank];
    // Disassemble bank
    disasm.list.clear(false);  // don't fully de-allocate space
    DiStella distella(*this, disasm.list, info, settings,
                      myDisLabels, myDisDirectives, myReserved, true);

    buf << "      ORG $" << HEX4 << info.offset << "\n\n";

    // Format in 'distella' style
    for(uInt32 i = 0; i < disasm.list.size(); ++i)
    {
      const DisassemblyTag& tag = disasm.list[i];

      // Add label (if any)
      if(tag.label != "")
        buf << ALIGN(7) << (tag.label+":");
      else
        buf << "       ";

      switch(tag.type)
      {
        case CartDebug::CODE:
        {
          buf << ALIGN(19) << tag.disasm << tag.ccount << "\n";
          break;
        }
        case CartDebug::NONE:
        {
          buf << "\n";
          break;
        }
        case CartDebug::ROW:
        {
          buf << tag.disasm << "\n";
          break;
        }
        case CartDebug::GFX:
        {
          buf << tag.disasm.substr(0, 9) << " ; |";
          for(int i = 12; i < 20; ++i)
            buf << ((tag.disasm[i] == '\x1e') ? "#" : " ");
          buf << "| $" << HEX4 << tag.address << " (G)\n";
          break;
        }
        case CartDebug::PGFX:
        {
          buf << tag.disasm.substr(0, 9) << " ; |";
          for(int i = 12; i < 20; ++i)
            buf << ((tag.disasm[i] == '\x1f') ? "*" : " ");
          buf << "| $" << HEX4 << tag.address << " (P)\n";
          break;
        }
        case CartDebug::DATA:
        {
          buf << tag.disasm.substr(0, 9) << " ;  $" << HEX4 << tag.address << " (D)\n";
          break;
        }
        default:
        {
          buf << "\n";
          break;
        }
      }
    }
  }

  cout << buf.str() << endl;  // TODO - redirect to file

  return DebuggerParser::red("not save to file yet");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::listConfig(int bank)
{
  uInt32 startbank = 0, endbank = bankCount();
  if(bank >= 0 && bank < bankCount())
  {
    startbank = bank;
    endbank = startbank + 1;
  }

  ostringstream buf;
  buf << "(items marked '*' are user-defined)" << endl;
  for(uInt32 b = startbank; b < endbank; ++b)
  {
    BankInfo& info = myBankInfo[b];
    buf << "[" << b << "]" << endl;
    for(DirectiveList::const_iterator i = info.directiveList.begin();
        i != info.directiveList.end(); ++i)
    {
      if(i->type != CartDebug::NONE)
      {
        buf << "(*) ";
        disasmTypeAsString(buf, i->type);
        buf << " " << HEX4 << i->start << " " << HEX4 << i->end << endl;
      }
    }
    getBankDirectives(buf, info);
  }

  return buf.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string CartDebug::clearConfig(int bank)
{
  uInt32 startbank = 0, endbank = bankCount();
  if(bank >= 0 && bank < bankCount())
  {
    startbank = bank;
    endbank = startbank + 1;
  }

  uInt32 count = 0;
  for(uInt32 b = startbank; b < endbank; ++b)
  {
    count += myBankInfo[b].directiveList.size();
    myBankInfo[b].directiveList.clear();
  }

  ostringstream buf;
  if(count > 0)
    buf << "removed " << dec << count << " directives from "
        << dec << (endbank - startbank) << " banks";
  else
    buf << "no directives present";
  return buf.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::getCompletions(const char* in, StringList& completions) const
{
  // First scan system equates
  for(uInt16 addr = 0x00; addr <= 0x3F; ++addr)
    if(BSPF_startsWithIgnoreCase(ourTIAMnemonicR[addr], in))
      completions.push_back(ourTIAMnemonicR[addr]);
  for(uInt16 addr = 0x00; addr <= 0x7F; ++addr)
    if(BSPF_startsWithIgnoreCase(ourTIAMnemonicW[addr], in))
      completions.push_back(ourTIAMnemonicW[addr]);
  for(uInt16 addr = 0; addr <= 0x297-0x280; ++addr)
    if(BSPF_startsWithIgnoreCase(ourIOMnemonic[addr], in))
      completions.push_back(ourIOMnemonic[addr]);

  // Now scan user-defined labels
  LabelToAddr::const_iterator iter;
  for(iter = myUserAddresses.begin(); iter != myUserAddresses.end(); ++iter)
  {
    const char* l = iter->first.c_str();
    if(BSPF_startsWithIgnoreCase(l, in))
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
void CartDebug::getBankDirectives(ostream& buf, BankInfo& info) const
{
  // Disassemble the bank, then scan it for an up-to-date description
  DisassemblyList list;
  DiStella distella(*this, list, info, DiStella::settings,
                    (uInt8*)myDisLabels, (uInt8*)myDisDirectives,
                    (ReservedEquates&)myReserved, true);

  if(list.size() == 0)
    return;

  // Start with the offset for this bank
  buf << "ORG " << HEX4 << info.offset << endl;

  DisasmType type = list[0].type;
  uInt16 start = list[0].address, last = list[1].address;
  if(start == 0) start = info.offset;
  for(uInt32 i = 1; i < list.size(); ++i)
  {
    const DisassemblyTag& tag = list[i];

    if(tag.type == CartDebug::NONE)
      continue;
    else if(tag.type != type)  // new range has started
    {
      // If switching data ranges, make sure the endpoint is valid
      // This is necessary because DATA sections don't always generate
      // consecutive numbers/addresses for the range
      last = tag.address - 1;

      disasmTypeAsString(buf, type);
      buf << " " << HEX4 << start << " " << HEX4 << last << endl;

      type = tag.type;
      start = last = tag.address;
    }
    else
      last = tag.address;
  }
  // Grab the last directive, making sure it accounts for all remaining space
  disasmTypeAsString(buf, type);
  buf << " " << HEX4 << start << " " << HEX4 << (info.offset+info.end) << endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::addressTypeAsString(ostream& buf, uInt16 addr) const
{
  if(!(addr & 0x1000))
  {
    buf << DebuggerParser::red("type only defined for cart address space");
    return;
  }

  uInt8 directive = myDisDirectives[addr & 0xFFF] & 0xFC,
        debugger  = myDebugger.getAccessFlags(addr) & 0xFC,
        label     = myDisLabels[addr & 0xFFF];

  buf << endl << "directive: " << myDebugger.valueToString(directive, kBASE_2_8) << " ";
  disasmTypeAsString(buf, directive);
  buf << endl << "emulation: " << myDebugger.valueToString(debugger, kBASE_2_8) << " ";
  disasmTypeAsString(buf, debugger);
  buf << endl << "tentative: " << myDebugger.valueToString(label, kBASE_2_8) << " ";
  disasmTypeAsString(buf, label);
  buf << endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::disasmTypeAsString(ostream& buf, DisasmType type) const
{
  switch(type)
  {
    case CartDebug::CODE:   buf << "CODE";   break;
    case CartDebug::GFX:    buf << "GFX";    break;
    case CartDebug::PGFX:   buf << "PGFX";   break;
    case CartDebug::DATA:   buf << "DATA";   break;
    case CartDebug::ROW:    buf << "ROW";    break;
    case CartDebug::REFERENCED:
    case CartDebug::VALID_ENTRY:
    case CartDebug::NONE:                    break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartDebug::disasmTypeAsString(ostream& buf, uInt8 flags) const
{
  if(flags)
  {
    if(flags & CartDebug::CODE)
      buf << "CODE ";
    if(flags & CartDebug::GFX)
      buf << "GFX ";
    if(flags & CartDebug::PGFX)
      buf << "PGFX ";
    if(flags & CartDebug::DATA)
      buf << "DATA ";
    if(flags & CartDebug::ROW)
      buf << "ROW ";
    if(flags & CartDebug::REFERENCED)
      buf << "*REFERENCED ";
    if(flags & CartDebug::VALID_ENTRY)
      buf << "*VALID_ENTRY ";
  }
  else
    buf << "no flags set";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CartDebug::ourTIAMnemonicR[64] = {
// Standard $00-based TIA read locations:
  "CXM0P", "CXM1P", "CXP0FB", "CXP1FB", "CXM0FB", "CXM1FB", "CXBLPF", "CXPPMM",
  "INPT0", "INPT1", "INPT2", "INPT3", "INPT4", "INPT5", "$0E", "$0F",
// Mirrored $10-based TIA read locations:
  "CXM0P.10", "CXM1P.10", "CXP0FB.10", "CXP1FB.10", "CXM0FB.10", "CXM1FB.10",
  "CXBLPF.10", "CXPPMM.10", "INPT0.10", "INPT1.10", "INPT2.10", "INPT3.10",
  "INPT4.10", "INPT5.10", "$1E", "$1F",
// Mirrored $20-based TIA read locations:
  "CXM0P.20", "CXM1P.20", "CXP0FB.20", "CXP1FB.20", "CXM0FB.20", "CXM1FB.20",
  "CXBLPF.20", "CXPPMM.20", "INPT0.20", "INPT1.20", "INPT2.20", "INPT3.20",
  "INPT4.20", "INPT5.20", "$2E", "$2F",
// Mirrored $30-based TIA read locations:
  "CXM0P.30", "CXM1P.30", "CXP0FB.30", "CXP1FB.30", "CXM0FB.30", "CXM1FB.30",
  "CXBLPF.30", "CXPPMM.30", "INPT0.30", "INPT1.30", "INPT2.30", "INPT3.30",
  "INPT4.30", "INPT5.30", "$3E", "$3F",
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CartDebug::ourTIAMnemonicW[128] = {
// Standard $00-based TIA write locations:
  "VSYNC", "VBLANK", "WSYNC", "RSYNC", "NUSIZ0", "NUSIZ1", "COLUP0", "COLUP1",
  "COLUPF", "COLUBK", "CTRLPF", "REFP0", "REFP1", "PF0", "PF1", "PF2",
  "RESP0", "RESP1", "RESM0", "RESM1", "RESBL", "AUDC0", "AUDC1", "AUDF0",
  "AUDF1", "AUDV0", "AUDV1", "GRP0", "GRP1", "ENAM0", "ENAM1", "ENABL",
  "HMP0", "HMP1", "HMM0", "HMM1", "HMBL", "VDELP0", "VDELP1", "VDELBL",
  "RESMP0", "RESMP1", "HMOVE", "HMCLR", "CXCLR", "$2D", "$2E", "$2F",
  "$30", "$31", "$32", "$33", "$34", "$35", "$36", "$37",
  "$38", "$39", "$3A", "$3B", "$3C", "$3D", "$3E", "$3F",
// Mirrored $40-based TIA write locations:
  "VSYNC.40", "VBLANK.40", "WSYNC.40", "RSYNC.40", "NUSIZ0.40", "NUSIZ1.40",
  "COLUP0.40", "COLUP1.40", "COLUPF.40", "COLUBK.40", "CTRLPF.40", "REFP0.40",
  "REFP1.40", "PF0.40", "PF1.40", "PF2.40", "RESP0.40", "RESP1.40", "RESM0.40",
  "RESM1.40", "RESBL.40", "AUDC0.40", "AUDC1.40", "AUDF0.40", "AUDF1.40",
  "AUDV0.40", "AUDV1.40", "GRP0.40", "GRP1.40", "ENAM0.40", "ENAM1.40",
  "ENABL.40", "HMP0.40", "HMP1.40", "HMM0.40", "HMM1.40", "HMBL.40",
  "VDELP0.40", "VDELP1.40", "VDELBL.40", "RESMP0.40", "RESMP1.40", "HMOVE.40",
  "HMCLR.40", "CXCLR.40", "$6D", "$6E", "$6F",
  "$70", "$71", "$72", "$73", "$74", "$75", "$76", "$77",
  "$78", "$79", "$7A", "$7B", "$7C", "$7D", "$7E", "$7F"
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* CartDebug::ourIOMnemonic[24] = {
  "SWCHA", "SWACNT", "SWCHB", "SWBCNT", "INTIM", "TIMINT", "$0286", "$0287",
  "$0288", "$0289", "$028A", "$028B", "$028C", "$028D", "$028E", "$028F",
  "$0290", "$0291", "$0292", "$0293", "TIM1T", "TIM8T", "TIM64T", "T1024T"
};
