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
// $Id: DebuggerDialog.cxx,v 1.28 2005-08-01 22:33:15 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "Widget.hxx"
#include "Dialog.hxx"
#include "TabWidget.hxx"
#include "TiaOutputWidget.hxx"
#include "TiaInfoWidget.hxx"
#include "PromptWidget.hxx"
#include "CpuWidget.hxx"
#include "RamWidget.hxx"
#include "TiaWidget.hxx"
#include "CheatWidget.hxx"
#include "Rect.hxx"
#include "Debugger.hxx"

#include "DebuggerDialog.hxx"

enum {
  kDDStepCmd  = 'DDst',
  kDDTraceCmd = 'DDtr',
  kDDAdvCmd   = 'DDav',
  kDDSAdvCmd  = 'DDsv',
  kDDExitCmd  = 'DDex'
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DebuggerDialog::DebuggerDialog(OSystem* osystem, DialogContainer* parent,
                               int x, int y, int w, int h)
  : Dialog(osystem, parent, x, y, w, h),
    myTab(NULL)
{
  addTiaArea();
  addTabArea();
  addStatusArea();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DebuggerDialog::~DebuggerDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::loadConfig()
{
cerr << "DebuggerDialog::loadConfig()\n";
  myTab->loadConfig();
  myTiaInfo->loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::handleKeyDown(int ascii, int keycode, int modifiers)
{
  // Doing this means we disallow 'Alt xxx' events to any widget in the tabset
  if(instance()->eventHandler().kbdAlt(modifiers))
  {
    if(ascii == 's')
      doStep();
    else if(ascii == 't')
      doTrace();
    else if(ascii == 'f')
      doAdvance();
    else if(ascii == 'l')
      doScanlineAdvance();
  }
  else
    myTab->handleKeyDown(ascii, keycode, modifiers);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::handleCommand(CommandSender* sender, int cmd,
                                   int data, int id)
{
  // We reload the tabs in the cases where the actions could possibly
  // change their contents
  switch(cmd)
  {
    case kDDStepCmd:
      doStep();
      break;

    case kDDTraceCmd:
      doTrace();
      break;

    case kDDAdvCmd:
      doAdvance();
      break;

    case kDDSAdvCmd:
      doScanlineAdvance();
      break;

    case kDDExitCmd:
      doExit();
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::addTiaArea()
{
  GUI::Rect r = instance()->debugger().getTiaBounds();

  myTiaOutput = new TiaOutputWidget(this, r.left, r.top, r.width(), r.height());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::addTabArea()
{
  const int vBorder = 4;
  const int vWidth = _w - kButtonWidth - 20;

  GUI::Rect r = instance()->debugger().getTabBounds();

  // The tab widget
  myTab = new TabWidget(this, r.left, r.top + vBorder, vWidth,
                        r.height() - vBorder);

  // 1) The Prompt/console tab
  myTab->addTab("Prompt");
  myPrompt = new PromptWidget(myTab, 2, 2, vWidth - vBorder,
                              r.height() - 25);
  myTab->setParentWidget(0, myPrompt, myPrompt);

  // 2) The CPU tab
  myTab->addTab("CPU");
  CpuWidget* cpu = new CpuWidget(myTab, 2, 2, vWidth - vBorder,
                                 r.height() - 25);
  myTab->setParentWidget(1, cpu, cpu->activeWidget());

  // 3) The RAM tab (part of RIOT)
  myTab->addTab("RAM");
  RamWidget* ram = new RamWidget(myTab, 2, 2, vWidth - vBorder,
                                 r.height() - 25);
  myTab->setParentWidget(2, ram, ram->activeWidget());

  // 4) The input/output tab (part of RIOT)
  myTab->addTab("I/O");


  // 5) The TIA tab
  myTab->addTab("TIA");
  TiaWidget* tia = new TiaWidget(myTab, 2, 2, vWidth - vBorder,
                                 r.height() - 25);
  myTab->setParentWidget(4, tia, tia->activeWidget());


  // 6) The ROM tab
  myTab->addTab("ROM");


  // 7) The Cheat tab
  myTab->addTab("Cheat");
  CheatWidget* cheat = new CheatWidget(myTab, 2, 2,
                                       vWidth - vBorder, r.height() - 25);
  myTab->setParentWidget(6, cheat, cheat->activeWidget());

  // Set active tab to prompt
  myTab->setActiveTab(0);

  // Add some buttons that are always shown, no matter which tab we're in
  int yoff = r.top + vBorder + kTabHeight + 5;
  addButton(vWidth + 10, yoff, "Step", kDDStepCmd, 0);
  yoff += 22;
  addButton(vWidth + 10, yoff, "Trace", kDDTraceCmd, 0);
  yoff += 22;
  addButton(vWidth + 10, yoff, "Scan +1", kDDSAdvCmd, 0);
  yoff += 22;
  addButton(vWidth + 10, yoff, "Frame +1", kDDAdvCmd, 0);

  addButton(vWidth + 10, r.bottom - 22 - 10, "Exit", kDDExitCmd, 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::addStatusArea()
{
  GUI::Rect r = instance()->debugger().getStatusBounds();
  myTiaInfo = new TiaInfoWidget(this, r.left, r.top, r.width(), r.height());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::doStep()
{
  instance()->debugger().step();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::doTrace()
{
  instance()->debugger().trace();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::doAdvance()
{
  instance()->debugger().nextFrame(1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::doScanlineAdvance()
{
  instance()->debugger().nextScanline(1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerDialog::doExit()
{
  instance()->debugger().quit();
}
