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
// Copyright (c) 1995-2010 by Bradford W. Mott and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <sstream>

#include "Debugger.hxx"
#include "DebuggerParser.hxx"
#include "CartDebug.hxx"
#include "CpuDebug.hxx"
#include "DataGridWidget.hxx"
#include "PackedBitArray.hxx"
#include "GuiObject.hxx"
#include "InputTextDialog.hxx"
#include "EditTextWidget.hxx"
#include "ContextMenu.hxx"
#include "RomListWidget.hxx"
#include "RomWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomWidget::RomWidget(GuiObject* boss, const GUI::Font& font, int x, int y)
  : Widget(boss, font, x, y, 16, 16),
    CommandSender(boss),
    myListIsDirty(true),
    myCurrentBank(-1)
{
  _type = kRomWidget;

  int xpos, ypos;
  StaticTextWidget* t;

  // Create bank editable area
  xpos = x + 40;  ypos = y + 7;
  t = new StaticTextWidget(boss, font, xpos, ypos,
                           font.getStringWidth("Current bank: "),
                           font.getFontHeight(),
                           "Current bank:", kTextAlignLeft);

  xpos += t->getWidth() + 10;
  myBank = new DataGridWidget(boss, font, xpos, ypos-2,
                              1, 1, 4, 8, kBASE_10);
  myBank->setTarget(this);
  myBank->setRange(0, instance().debugger().cartDebug().bankCount());
  if(instance().debugger().cartDebug().bankCount() <= 1)
    myBank->setEditable(false);
  addFocusWidget(myBank);

  // Show number of banks
  xpos += myBank->getWidth() + 45;
  t = new StaticTextWidget(boss, font, xpos, ypos,
                           font.getStringWidth("Total banks: "),
                           font.getFontHeight(),
                           "Total banks:", kTextAlignLeft);

  xpos += t->getWidth() + 10;
  myBankCount = new EditTextWidget(boss, font, xpos, ypos-2,
                                   font.getStringWidth("XXXX"),
                                   font.getLineHeight(), "");
  myBankCount->setEditable(false);

  // Create rom listing
  xpos = x;  ypos += myBank->getHeight() + 4;
  GUI::Rect dialog = instance().debugger().getDialogBounds();
  int w = dialog.width() - x - 5, h = dialog.height() - ypos - 3;

  myRomList = new RomListWidget(boss, font, xpos, ypos, w, h);
  myRomList->setTarget(this);
  myRomList->myMenu->setTarget(this);
  myRomList->setStyle(kSolidFill);
  addFocusWidget(myRomList);

  // Calculate real dimensions
  _w = myRomList->getWidth();
  _h = myRomList->getHeight();

  // Create dialog box for save ROM (get name)
  StringList label;
  label.push_back("Filename: ");
  mySaveRom = new InputTextDialog(boss, font, label);
  mySaveRom->setTarget(this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomWidget::~RomWidget()
{
  myAddrList.clear();
  myLineList.clear();

  delete mySaveRom;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  switch(cmd)
  {
    case kListItemChecked:
      setBreak(data);
      break;

    case kListItemDataChangedCmd:
      patchROM(data, myRomList->getSelectedString());
      break;

    case kCMenuItemSelectedCmd:
    {
      const string& rmb = myRomList->myMenu->getSelectedTag();

      if(rmb == "saverom")
      {
        mySaveRom->show(_x + 50, _y + 80);
        mySaveRom->setTitle("");
        mySaveRom->setEmitSignal(kRomNameEntered);
      }
      else if(rmb == "setpc")
        setPC(myRomList->getSelected());

      break;
    }

    case kRomNameEntered:
    {
      const string& rom = mySaveRom->getResult();
      if(rom == "")
        mySaveRom->setTitle("Invalid name");
      else
      {
        saveROM(rom);
        parent().removeDialog();
      }
      break;
    }

    case kDGItemDataChangedCmd:
    {
      int bank = myBank->getSelectedValue();
      instance().debugger().setBank(bank);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::loadConfig()
{
  Debugger& dbg = instance().debugger();
  CartDebug& cart = dbg.cartDebug();
  bool bankChanged = myCurrentBank != cart.getBank();
  myCurrentBank = cart.getBank();

  // Fill romlist the current bank of source or disassembly
  // Only reload full bank when necessary
  if(myListIsDirty || bankChanged)
  {
    // Clear old mappings
    myAddrList.clear();
    myLineList.clear();

    StringList label, data, disasm;
    BoolArray state;

    // Disassemble zero-page RAM and entire bank and reset breakpoints
    cart.disassemble(myAddrList, label, data, disasm, 0x80, 0xff);
    cart.disassemble(myAddrList, label, data, disasm, 0xf000, 0xffff);

    PackedBitArray& bp = dbg.breakpoints();
    for(unsigned int i = 0; i < data.size(); ++i)
    {
      if(bp.isSet(myAddrList[i]))
        state.push_back(true);
      else
        state.push_back(false);
    }

    // Create a mapping from addresses to line numbers
    for(unsigned int i = 0; i < myAddrList.size(); ++i)
      myLineList.insert(make_pair(myAddrList[i], i));

    myRomList->setList(label, data, disasm, state);

    // Restore the old bank, in case we inadvertently switched while reading.
    dbg.setBank(myCurrentBank);

    myListIsDirty = false;
  }

  // Update romlist to point to current PC
  // Take mirroring of PC into account, as well as zero-page RAM
  int pc = dbg.cpuDebug().pc();
  if(pc & 0x1000) pc |= 0xe000;
  AddrToLine::iterator iter = myLineList.find(pc);

  // if current PC not found, do an update (we're executing what
  // we thought was an operand)

  // This doesn't help, and seems to actually hurt.
  /*
  if(iter == myLineList.end()) {
    incrementalUpdate(myRomList->currentPos(), myRomList->rows());
    iter = myLineList.find(pc);
  }
  */

  if(iter != myLineList.end())
    myRomList->setHighlighted(iter->second);

  // Set current bank
  IntArray alist;
  IntArray vlist;
  BoolArray changed;

  alist.push_back(-1);
  vlist.push_back(cart.getBank());
  changed.push_back(bankChanged);
  myBank->setList(alist, vlist, changed);

  // Indicate total number of banks
  myBankCount->setEditString(dbg.valueToString(cart.bankCount(), kBASE_10));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::setBreak(int data)
{
  bool state = myRomList->getState(data);
  instance().debugger().setBreakPoint(myAddrList[data], state);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::setPC(int data)
{
  ostringstream command;
  command << "pc #" << myAddrList[data];
  instance().debugger().run(command.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::patchROM(int data, const string& bytes)
{
  ostringstream command;

  // Temporarily set to base 16, since that's the format the disassembled
  // byte string is in.  This eliminates the need to prefix each byte with
  // a '$' character
  BaseFormat oldbase = instance().debugger().parser().base();
  instance().debugger().parser().setBase(kBASE_16);

  command << "rom #" << myAddrList[data] << " " << bytes;
  instance().debugger().run(command.str());

  // Restore previous base
  instance().debugger().parser().setBase(oldbase);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::saveROM(const string& rom)
{
  ostringstream command;
  command << "saverom " << rom;
  instance().debugger().run(command.str());
}