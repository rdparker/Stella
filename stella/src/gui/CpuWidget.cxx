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
// $Id: CpuWidget.cxx,v 1.1 2005-06-20 18:32:12 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <sstream>

#include "OSystem.hxx"
#include "GuiUtils.hxx"
#include "GuiObject.hxx"
#include "Debugger.hxx"
#include "Widget.hxx"
#include "DataGridWidget.hxx"

#include "CpuWidget.hxx"

enum {
  kPCRegAddr,
  kSPRegAddr,
  kARegAddr,
  kXRegAddr,
  kYRegAddr,
  kNumRegs
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CpuWidget::CpuWidget(GuiObject* boss, int x, int y, int w, int h)
  : Widget(boss, x, y, w, h),
    CommandSender(boss)
{
  int xpos   = 10;
  int ypos   = 20;
  int lwidth = 30;
  const int vWidth = _w - kButtonWidth - 20, space = 6, buttonw = 24;

/*
  // Create a 16x8 grid (16 x 8 = 128 RAM bytes) with labels
  for(int row = 0; row < 8; ++row)
  {
    StaticTextWidget* t = new StaticTextWidget(boss, xpos, ypos + row*kLineHeight + 2,
                          lwidth, kLineHeight,
                          Debugger::to_hex_16(row*16 + kRamStart) + string(":"),
                          kTextAlignLeft);
    t->setFont(instance()->consoleFont());
  }
  for(int col = 0; col < 16; ++col)
  {
    StaticTextWidget* t = new StaticTextWidget(boss, xpos + col*kColWidth + lwidth + 12,
                          ypos - kLineHeight,
                          lwidth, kLineHeight,
                          Debugger::to_hex_4(col),
                          kTextAlignLeft);
    t->setFont(instance()->consoleFont());
  }
*/  
  myCpuGrid = new DataGridWidget(boss, xpos+lwidth + 5, ypos, 1, 5, 8,
                                 0xffff);
  myCpuGrid->setTarget(this);
  myActiveWidget = myCpuGrid;

/*
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
*/
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CpuWidget::~CpuWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CpuWidget::handleCommand(CommandSender* sender, int cmd, int data)
{
  // We simply change the values in the ByteGridWidget
  // It will then send the 'kBGItemDataChangedCmd' signal to change the actual
  // memory location
  int addr, value;
  unsigned char byte;

  switch(cmd)
  {
    case kDGItemDataChangedCmd:
cerr << "info changed\n";
/*
      addr  = myRamGrid->getSelectedAddr() - kRamStart;
      value = myRamGrid->getSelectedValue();
      instance()->debugger().writeRAM(addr, value);
*/
      break;

/*
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
*/
  }

  instance()->frameBuffer().refresh();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CpuWidget::loadConfig()
{
  fillGrid();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CpuWidget::fillGrid()
{
  AddrList alist;
  ValueList vlist;

  for(unsigned int i = 0; i < kNumRegs; i++)
  {
    alist.push_back(i*16);
    vlist.push_back(i*16);//instance()->debugger().readRAM(i));
  }

  myCpuGrid->setList(alist, vlist);
}
