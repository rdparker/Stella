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
// $Id: RomWidget.cxx,v 1.5 2005-08-24 22:01:45 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <sstream>

#include "Debugger.hxx"
#include "GuiObject.hxx"
#include "CheckListWidget.hxx"
#include "RomWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomWidget::RomWidget(GuiObject* boss, const GUI::Font& font, int x, int y)
  : Widget(boss, x, y, 16, 16),
    CommandSender(boss),
    myFirstLoad(true),
    mySourceAvailable(false),
    myCurrentBank(-1)
{
  int w = 58 * font.getMaxCharWidth(),
      h = 31 * font.getLineHeight();

  myRomList = new CheckListWidget(boss, font, x, y, w, h);
  myRomList->setTarget(this);
  myRomList->setStyle(kSolidFill);
  addFocusWidget(myRomList);

  // Calculate real dimensions
  _w = myRomList->getWidth();
  _h = myRomList->getHeight();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomWidget::~RomWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  switch(cmd)
  {
    case kListScrolledCmd:
      incrementalUpdate();
      break;

    case kListItemChecked:
      cerr << "(un)set a breakpoint at address " << data << endl;
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::loadConfig()
{
  /* FIXME
     We need logic here to only fill the grid at startup and when
     bankswitching.  At other times, we receive 'kListScrolledCmd'
     command, which means the current romlist view is invalid and
     should be filled with new data.
  */
cerr << "RomWidget::loadConfig()\n";
  // Only reload full bank when necessary
  if(myFirstLoad || myCurrentBank != instance()->debugger().getBank())
  {
    initialUpdate();
    myFirstLoad = false;
  }
  else  // only reload what's in current view
  {
    incrementalUpdate();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::initialUpdate()
{
  Debugger& dbg = instance()->debugger();

  myCurrentBank = dbg.getBank();

  // Fill romlist with entire ROM (FIXME - only fill with current bank)
  if(mySourceAvailable)
    ; // FIXME
  else
  {
    StringList addr, data;
    BoolArray  state;

    // Disassemble entire bank (up to 4096 lines)
    dbg.disassemble(addr, data, 0xf000, 4096);
    for(unsigned int i = 0; i < data.size(); ++i)
      state.push_back(false);

    myRomList->setList(data, state);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomWidget::incrementalUpdate()
{
}
