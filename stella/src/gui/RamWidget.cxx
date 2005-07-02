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
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: RamWidget.cxx,v 1.10 2005-07-02 18:34:54 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <sstream>

#include "OSystem.hxx"
#include "FrameBuffer.hxx"
#include "GuiUtils.hxx"
#include "GuiObject.hxx"
#include "Debugger.hxx"
#include "Widget.hxx"
#include "EditTextWidget.hxx"
#include "DataGridWidget.hxx"

#include "RamWidget.hxx"

enum {
  kRZeroCmd   = 'RWze',
  kRInvertCmd = 'RWiv',
  kRNegateCmd = 'RWng',
  kRIncCmd    = 'RWic',
  kRDecCmd    = 'RWdc',
  kRShiftLCmd = 'RWls',
  kRShiftRCmd = 'RWrs'
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RamWidget::RamWidget(GuiObject* boss, int x, int y, int w, int h)
  : Widget(boss, x, y, w, h),
    CommandSender(boss)
{
  int xpos   = 10;
  int ypos   = 20;
  int lwidth = 30;
  const int vWidth = _w - kButtonWidth - 20, space = 6, buttonw = 24;
  const GUI::Font& font = instance()->consoleFont();

  // Create a 16x8 grid holding byte values (16 x 8 = 128 RAM bytes) with labels
  myRamGrid = new DataGridWidget(boss, xpos+lwidth + 5, ypos, 16, 8, 2, 0xff, kBASE_16);
  myRamGrid->setTarget(this);
  myRamGrid->clearFlags(WIDGET_TAB_NAVIGATE);
  myActiveWidget = myRamGrid;

  for(int row = 0; row < 8; ++row)
  {
    StaticTextWidget* t = new StaticTextWidget(boss, xpos, ypos + row*kLineHeight + 2,
                          lwidth, kLineHeight,
                          Debugger::to_hex_16(row*16 + kRamStart) + string(":"),
                          kTextAlignLeft);
    t->setFont(font);
  }
  for(int col = 0; col < 16; ++col)
  {
    StaticTextWidget* t = new StaticTextWidget(boss,
                          xpos + col*myRamGrid->colWidth() + lwidth + 12,
                          ypos - kLineHeight,
                          lwidth, kLineHeight,
                          Debugger::to_hex_4(col),
                          kTextAlignLeft);
    t->setFont(font);
  }
  
  xpos = 20;  ypos = 11 * kLineHeight;
  new StaticTextWidget(boss, xpos, ypos, 30, kLineHeight, "Label: ", kTextAlignLeft);
  xpos += 30;
  myLabel = new EditTextWidget(boss, xpos, ypos-2, 100, kLineHeight, "");
  myLabel->clearFlags(WIDGET_TAB_NAVIGATE);
  myLabel->setFont(font);
  myLabel->setEditable(false);

  xpos += 120;
  new StaticTextWidget(boss, xpos, ypos, 35, kLineHeight, "Decimal: ", kTextAlignLeft);
  xpos += 35;
  myDecValue = new EditTextWidget(boss, xpos, ypos-2, 30, kLineHeight, "");
  myDecValue->clearFlags(WIDGET_TAB_NAVIGATE);
  myDecValue->setFont(font);
  myDecValue->setEditable(false);

  xpos += 48;
  new StaticTextWidget(boss, xpos, ypos, 35, kLineHeight, "Binary: ", kTextAlignLeft);
  xpos += 35;
  myBinValue = new EditTextWidget(boss, xpos, ypos-2, 60, kLineHeight, "");
  myBinValue->clearFlags(WIDGET_TAB_NAVIGATE);
  myBinValue->setFont(font);
  myBinValue->setEditable(false);

  // Add some buttons for common actions
  ButtonWidget* b;
  xpos = vWidth + 10;  ypos = 20;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "0", kRZeroCmd, 0);
  b->setTarget(this);

  ypos += 16 + space;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "Inv", kRInvertCmd, 0);
  b->setTarget(this);

  ypos += 16 + space;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "++", kRIncCmd, 0);
  b->setTarget(this);

  ypos += 16 + space;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "<<", kRShiftLCmd, 0);
  b->setTarget(this);

  xpos = vWidth + 30 + 10;  ypos = 20;
//  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "", kRCmd, 0);
//  b->setTarget(this);

  ypos += 16 + space;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "Neg", kRNegateCmd, 0);
  b->setTarget(this);

  ypos += 16 + space;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, "--", kRDecCmd, 0);
  b->setTarget(this);

  ypos += 16 + space;
  b = new ButtonWidget(boss, xpos, ypos, buttonw, 16, ">>", kRShiftRCmd, 0);
  b->setTarget(this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RamWidget::~RamWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RamWidget::handleCommand(CommandSender* sender, int cmd, int data)
{
  // We simply change the values in the ByteGridWidget
  // It will then send the 'kDGItemDataChangedCmd' signal to change the actual
  // memory location
  int addr, value;
  unsigned char byte;
  const char* buf;

  switch(cmd)
  {
    case kDGItemDataChangedCmd:
      addr  = myRamGrid->getSelectedAddr();
      value = myRamGrid->getSelectedValue();

      instance()->debugger().writeRAM(addr - kRamStart, value);
      myDecValue->setEditString(instance()->debugger().valueToString(value, kBASE_10));
      myBinValue->setEditString(instance()->debugger().valueToString(value, kBASE_2));
      break;

    case kDGSelectionChangedCmd:
      addr  = myRamGrid->getSelectedAddr();
      value = myRamGrid->getSelectedValue();

      buf = instance()->debugger().equates()->getLabel(addr);
      if(buf) myLabel->setEditString(buf);
      else    myLabel->setEditString("");

      myDecValue->setEditString(instance()->debugger().valueToString(value, kBASE_10));
      myBinValue->setEditString(instance()->debugger().valueToString(value, kBASE_2));
      break;

    case kRZeroCmd:
      myRamGrid->setSelectedValue(0);
      break;

    case kRInvertCmd:
      byte = (unsigned char) myRamGrid->getSelectedValue();
      byte = ~byte;
      myRamGrid->setSelectedValue((int)byte);
      break;

    case kRNegateCmd:
      byte = (unsigned char) myRamGrid->getSelectedValue();
      byte = (~byte) + 1;
      myRamGrid->setSelectedValue((int)byte);
      break;

    case kRIncCmd:
      byte = (unsigned char) myRamGrid->getSelectedValue();
      byte += 1;
      myRamGrid->setSelectedValue((int)byte);
      break;

    case kRDecCmd:
      byte = (unsigned char) myRamGrid->getSelectedValue();
      byte -= 1;
      myRamGrid->setSelectedValue((int)byte);
      break;

    case kRShiftLCmd:
      byte = (unsigned char) myRamGrid->getSelectedValue();
      byte <<= 1;
      myRamGrid->setSelectedValue((int)byte);
      break;

    case kRShiftRCmd:
      byte = (unsigned char) myRamGrid->getSelectedValue();
      byte >>= 1;
      myRamGrid->setSelectedValue((int)byte);
      break;
  }

  // TODO - dirty rect, or is it necessary here?
  instance()->frameBuffer().refreshOverlay();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RamWidget::loadConfig()
{
  fillGrid();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RamWidget::fillGrid()
{
  AddrList alist;
  ValueList vlist;
  BoolArray changed;

  Debugger& dbg = instance()->debugger();
  for(unsigned int i = 0; i < kRamSize; i++)
  {
    alist.push_back(kRamStart + i);
    vlist.push_back(dbg.readRAM(i));
    changed.push_back(dbg.ramChanged(i));
  }

  myRamGrid->setList(alist, vlist, changed);
}
