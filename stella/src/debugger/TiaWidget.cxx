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
// $Id: TiaWidget.cxx,v 1.5 2005-08-15 18:52:15 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "OSystem.hxx"
#include "FrameBuffer.hxx"
#include "GuiUtils.hxx"
#include "GuiObject.hxx"
#include "TIADebug.hxx"
#include "Widget.hxx"
#include "EditTextWidget.hxx"
#include "DataGridWidget.hxx"
#include "ColorWidget.hxx"
#include "ToggleBitWidget.hxx"
#include "TiaWidget.hxx"

// ID's for the various widgets
// We need ID's, since there are more than one of several types of widgets
enum {
  kRamID       = 'TWra',
  kColorRegsID = 'TWcr',
  kVSyncID     = 'TWvs',
  kVBlankID    = 'TWvb'
};

// Color registers
enum {
  kCOLUP0Addr,
  kCOLUP1Addr,
  kCOLUPFAddr,
  kCOLUBKAddr
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TiaWidget::TiaWidget(GuiObject* boss, int x, int y, int w, int h)
  : Widget(boss, x, y, w, h),
    CommandSender(boss)
{
  const GUI::Font& font = instance()->consoleFont();
  int xpos = 10, ypos = 25, lwidth = 4 * font.getMaxCharWidth();
  int fontHeight = font.getFontHeight(), lineHeight = font.getLineHeight();
  int fontWidth = font.getMaxCharWidth();
  StaticTextWidget* t;

  // Create a 16x1 grid holding byte values with labels
  myRamGrid = new DataGridWidget(boss, font, xpos + lwidth, ypos,
                                 16, 1, 2, 8, kBASE_16);
  myRamGrid->setEditable(false);
  myRamGrid->setTarget(this);
  myRamGrid->setID(kRamID);
  addFocusWidget(myRamGrid);

  t = new StaticTextWidget(boss, xpos, ypos + 2,
                           lwidth-2, fontHeight,
                           "00:", kTextAlignLeft);
  t->setFont(font);
  for(int col = 0; col < 16; ++col)
  {
    t = new StaticTextWidget(boss, xpos + col*myRamGrid->colWidth() + lwidth + 7,
                             ypos - lineHeight,
                             fontWidth, fontHeight,
                             Debugger::to_hex_4(col),
                             kTextAlignLeft);
    t->setFont(font);
  }
  
  xpos = 20;  ypos += 2 * lineHeight;
  t = new StaticTextWidget(boss, xpos, ypos,
                           6*fontWidth, fontHeight,
                           "Label:", kTextAlignLeft);
  t->setFont(font);
  xpos += 6*fontWidth + 5;
  myLabel = new EditTextWidget(boss, xpos, ypos-2, 15*fontWidth, lineHeight, "");
  myLabel->setFont(font);
  myLabel->setEditable(false);

  xpos += 15*fontWidth + 20;
  t = new StaticTextWidget(boss, xpos, ypos,
                           4*fontWidth, fontHeight,
                           "Dec:", kTextAlignLeft);
  t->setFont(font);
  xpos += 4*fontWidth + 5;
  myDecValue = new EditTextWidget(boss, xpos, ypos-2, 4*fontWidth, lineHeight, "");
  myDecValue->setFont(font);
  myDecValue->setEditable(false);

  xpos += 4*fontWidth + 20;
  t = new StaticTextWidget(boss, xpos, ypos,
                           4*fontWidth, fontHeight,
                           "Bin:", kTextAlignLeft);
  t->setFont(font);
  xpos += 4*fontWidth + 5;
  myBinValue = new EditTextWidget(boss, xpos, ypos-2, 9*fontWidth, lineHeight, "");
  myBinValue->setFont(font);
  myBinValue->setEditable(false);

  // Color registers
  const char* regNames[] = { "COLUP0:", "COLUP1:", "COLUPF:", "COLUBK:" };
  xpos = 10;  ypos += 2*lineHeight;
  for(int row = 0; row < 4; ++row)
  {
    t = new StaticTextWidget(boss, xpos, ypos + row*lineHeight + 2,
                             7*fontWidth, fontHeight,
                             regNames[row],
                             kTextAlignLeft);
    t->setFont(font);
  }
  xpos += 7*fontWidth + 5;
  myColorRegs = new DataGridWidget(boss, font, xpos, ypos,
                                   1, 4, 2, 8, kBASE_16);
  myColorRegs->setTarget(this);
  myColorRegs->setID(kColorRegsID);
  addFocusWidget(myColorRegs);

  xpos += myColorRegs->colWidth() + 5;
  myCOLUP0Color = new ColorWidget(boss, xpos, ypos+2, 20, lineHeight - 4);
  myCOLUP0Color->setTarget(this);

  ypos += lineHeight;
  myCOLUP1Color = new ColorWidget(boss, xpos, ypos+2, 20, lineHeight - 4);
  myCOLUP1Color->setTarget(this);

  ypos += lineHeight;
  myCOLUPFColor = new ColorWidget(boss, xpos, ypos+2, 20, lineHeight - 4);
  myCOLUPFColor->setTarget(this);

  ypos += lineHeight;
  myCOLUBKColor = new ColorWidget(boss, xpos, ypos+2, 20, lineHeight - 4);
  myCOLUBKColor->setTarget(this);

  // P0 register info
  xpos = 10;  ypos += 2*lineHeight;
  t = new StaticTextWidget(boss, xpos, ypos+2,
                           7*fontWidth, fontHeight,
                           "P0/ GR:", kTextAlignLeft);
  t->setFont(font);
  xpos += 7*fontWidth + 5;
  myGRP0 = new ToggleBitWidget(boss, font, xpos, ypos, 8, 1);
  myGRP0->setTarget(this);
  addFocusWidget(myGRP0);

  // Set the strings to be used in the PSRegister
  // We only do this once because it's the state that changes, not the strings
  const char* offstr[] = { "0", "0", "0", "0", "0", "0", "0", "0" };
  const char* onstr[]  = { "1", "1", "1", "1", "1", "1", "1", "1" };
  StringList off, on;
  for(int i = 0; i < 8; ++i)
  {
    off.push_back(offstr[i]);
    on.push_back(onstr[i]);
  }
  myGRP0->setList(off, on);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TiaWidget::~TiaWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaWidget::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  // We simply change the values in the ByteGridWidget
  // It will then send the 'kDGItemDataChangedCmd' signal to change the actual
  // memory location
  int addr, value;
  string buf;

  Debugger& dbg = instance()->debugger();

  switch(cmd)
  {
    case kDGItemDataChangedCmd:
      switch(id)
      {
        case kColorRegsID:
          changeColorRegs();
          break;

        default:
          cerr << "TiaWidget DG changed\n";
          break;
      }
      // FIXME -  maybe issue a full reload, since changing one item can affect
      //          others in this tab??
      // loadConfig();
      break;

    case kDGSelectionChangedCmd:
      switch(id)
      {
        case kRamID:
          addr  = myRamGrid->getSelectedAddr();
          value = myRamGrid->getSelectedValue();

          myLabel->setEditString(dbg.equates()->getLabel(addr));

          myDecValue->setEditString(dbg.valueToString(value, kBASE_10));
          myBinValue->setEditString(dbg.valueToString(value, kBASE_2));
          break;
      }
      break;

    case kCheckActionCmd:
      switch(id)
      {
        case kVSyncID:
          cerr << "vsync toggled\n";
          break;

        case kVBlankID:
          cerr << "vblank toggled\n";
          break;
      }
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaWidget::loadConfig()
{
cerr << "TiaWidget::loadConfig()\n";
  fillGrid();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaWidget::fillGrid()
{
  IntArray alist;
  IntArray vlist;
  BoolArray orig, changed;

  Debugger& dbg = instance()->debugger();
  TIADebug& tia = dbg.tiaDebug();
  TiaState state    = (TiaState&) tia.getState();
  TiaState oldstate = (TiaState&) tia.getOldState();

  // TIA RAM
  alist.clear();  vlist.clear();  changed.clear();
  for(unsigned int i = 0; i < 16; i++)
  {
    alist.push_back(i);
    vlist.push_back(state.ram[i]);
    changed.push_back(state.ram[i] != oldstate.ram[i]);
  }
  myRamGrid->setList(alist, vlist, changed);

  // Color registers
  alist.clear();  vlist.clear();  changed.clear();
  for(unsigned int i = 0; i < 4; i++)
  {
    alist.push_back(i);
    vlist.push_back(state.coluRegs[i]);
    changed.push_back(state.coluRegs[i] != oldstate.coluRegs[i]);
  }
  myColorRegs->setList(alist, vlist, changed);

  myCOLUP0Color->setColor(state.coluRegs[0]);
  myCOLUP1Color->setColor(state.coluRegs[1]);
  myCOLUPFColor->setColor(state.coluRegs[2]);
  myCOLUBKColor->setColor(state.coluRegs[3]);

  // GRP0 register
  BoolArray grNew, grOld;
  convertCharToBool(grNew, state.gr[P0]);
  convertCharToBool(grOld, oldstate.gr[P0]);

  changed.clear();
  for(unsigned int i = 0; i < 8; ++i)
    changed.push_back(grNew[i] != grOld[i]);

  myGRP0->setState(grNew, changed);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaWidget::convertCharToBool(BoolArray& b, unsigned char value)
{
  for(unsigned int i = 0; i < 8; ++i)
  {
    if(value & (1<<(7-i)))
      b.push_back(true);
    else
      b.push_back(false);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaWidget::changeColorRegs()
{
  int addr  = myColorRegs->getSelectedAddr();
  int value = myColorRegs->getSelectedValue();

  switch(addr)
  {
    case kCOLUP0Addr:
      instance()->debugger().tiaDebug().coluP0(value);
      myCOLUP0Color->setColor(value);
      break;

    case kCOLUP1Addr:
      instance()->debugger().tiaDebug().coluP1(value);
      myCOLUP1Color->setColor(value);
      break;

    case kCOLUPFAddr:
      instance()->debugger().tiaDebug().coluPF(value);
      myCOLUPFColor->setColor(value);
      break;

    case kCOLUBKAddr:
      instance()->debugger().tiaDebug().coluBK(value);
      myCOLUBKColor->setColor(value);
      break;
  }
}
