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
// Copyright (c) 1995-2005 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: OptionsDialog.cxx,v 1.14 2005-05-10 19:20:44 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "OSystem.hxx"
#include "Dialog.hxx"
#include "DialogContainer.hxx"
#include "Widget.hxx"
#include "Control.hxx"
#include "VideoDialog.hxx"
#include "AudioDialog.hxx"
#include "EventMappingDialog.hxx"
#include "GameInfoDialog.hxx"
#include "HelpDialog.hxx"
#include "OptionsDialog.hxx"

#include "bspf.hxx"

enum {
  kVidCmd   = 'VIDO',
  kAudCmd   = 'AUDO',
  kEMapCmd  = 'EMAP',
  kMiscCmd  = 'MISC',
  kInfoCmd  = 'INFO',
  kHelpCmd  = 'HELP',
};

enum {
  kRowHeight      = 22,
  kBigButtonWidth = 100,
  kMainMenuWidth  = (kBigButtonWidth + 2 * 8),
  kMainMenuHeight = 6 * kRowHeight + 10,
};

#define addBigButton(label, cmd, hotkey) \
	new ButtonWidget(this, xoffset, yoffset, kBigButtonWidth, 18, label, cmd, hotkey); yoffset += kRowHeight

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OptionsDialog::OptionsDialog(OSystem* osystem, DialogContainer* parent)
// FIXME - we have to initialize the video system at least once *before*
//         creating a new console.  For now, just use predefined values.
//         Eventually, this subsystem will have to take into account screen size changes
    : Dialog(osystem, parent,
            (osystem->frameBuffer().baseWidth() - kMainMenuWidth) / 2,
            (osystem->frameBuffer().baseHeight() - kMainMenuHeight)/2,
            kMainMenuWidth, kMainMenuHeight),
      myVideoDialog(NULL)
{
  int yoffset = 7;
  const int xoffset = (_w - kBigButtonWidth) / 2;

  addBigButton("Video Settings", kVidCmd, 'V');
#ifdef SOUND_SUPPORT
  addBigButton("Audio Settings", kAudCmd, 'A');
#else
  ButtonWidget* b = addBigButton("Audio Settings", kAudCmd, 'A');
  b->setEnabled(false);
#endif
  addBigButton("Event Remapping", kEMapCmd, 'E');
  addBigButton("Miscellaneous", kMiscCmd, 'M');
  addBigButton("Game Information", kInfoCmd, 'I');
  addBigButton("Help", kHelpCmd, 'H');

  // Set some sane values for the dialog boxes
  uInt16 fbWidth  = osystem->frameBuffer().baseWidth();
  uInt16 fbHeight = osystem->frameBuffer().baseHeight();
  uInt16 x, y, w, h;

  // Now create all the dialogs attached to each menu button
  w = 230; h = 130;
  checkBounds(fbWidth, fbHeight, &x, &y, &w, &h);
  myVideoDialog = new VideoDialog(myOSystem, parent, x, y, w, h);

  w = 200; h = 100;
  checkBounds(fbWidth, fbHeight, &x, &y, &w, &h);
  myAudioDialog = new AudioDialog(myOSystem, parent, x, y, w, h);

  w = 280; h = 170;
  checkBounds(fbWidth, fbHeight, &x, &y, &w, &h);
  myEventMappingDialog = new EventMappingDialog(myOSystem, parent, x, y, w, h);

//  w = 250; h = 150;
//  checkBounds(fbWidth, fbHeight, &x, &y, &w, &h);
//  myMiscDialog         = new MiscDialog(myOSystem, parent, x, y, w, h);

  w = 255; h = 150;
  checkBounds(fbWidth, fbHeight, &x, &y, &w, &h);
  myGameInfoDialog = new GameInfoDialog(myOSystem, parent, x, y, w, h);

  w = 255; h = 150;
  checkBounds(fbWidth, fbHeight, &x, &y, &w, &h);
  myHelpDialog = new HelpDialog(myOSystem, parent, x, y, w, h);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OptionsDialog::~OptionsDialog()
{
  delete myVideoDialog;
  delete myAudioDialog;
  delete myEventMappingDialog;
//  delete myMiscDialog;
  delete myGameInfoDialog;
  delete myHelpDialog;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OptionsDialog::checkBounds(uInt16 width, uInt16 height,
                                uInt16* x, uInt16* y, uInt16* w, uInt16* h)
{
  if(*w > width) *w = width;
  if(*h > height) *h = height;
  *x = (width - *w) / 2;
  *y = (height - *h) / 2;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OptionsDialog::handleCommand(CommandSender* sender, uInt32 cmd, uInt32 data)
{
  switch(cmd)
  {
    case kVidCmd:
      parent()->addDialog(myVideoDialog);
      break;

    case kAudCmd:
      parent()->addDialog(myAudioDialog);
      break;

    case kEMapCmd:
      parent()->addDialog(myEventMappingDialog);
      break;

    case kMiscCmd:
//      parent()->addDialog(myMiscDialog);
cerr << "push MiscDialog to top of stack\n";
      break;

    case kInfoCmd:
      parent()->addDialog(myGameInfoDialog);
      break;

    case kHelpCmd:
      parent()->addDialog(myHelpDialog);
      break;

    default:
      Dialog::handleCommand(sender, cmd, data);
  }
}
