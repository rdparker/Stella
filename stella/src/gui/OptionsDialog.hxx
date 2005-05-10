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
// $Id: OptionsDialog.hxx,v 1.5 2005-05-10 19:20:44 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#ifndef OPTIONS_DIALOG_HXX
#define OPTIONS_DIALOG_HXX

class Properties;
class CommandSender;
class Dialog;
class DialogContainer;
class VideoDialog;
class AudioDialog;
class EventMappingDialog;
class MiscDialog;
class HelpDialog;

#include "OSystem.hxx"
#include "GameInfoDialog.hxx"
#include "bspf.hxx"

class OptionsDialog : public Dialog
{
  public:
    OptionsDialog(OSystem* osystem, DialogContainer* parent);
    ~OptionsDialog();

    virtual void handleCommand(CommandSender* sender, uInt32 cmd, uInt32 data);

    void setGameProfile(Properties& props) { myGameInfoDialog->setGameProfile(props); }

  protected:
    VideoDialog*        myVideoDialog;
    AudioDialog*        myAudioDialog;
    EventMappingDialog* myEventMappingDialog;
    MiscDialog*         myMiscDialog;
    GameInfoDialog*     myGameInfoDialog;
    HelpDialog*         myHelpDialog;

  private:
    void checkBounds(uInt16 width, uInt16 height,
                     uInt16* x, uInt16* y, uInt16* w, uInt16* h);
};

#endif
