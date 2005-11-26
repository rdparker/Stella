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
// $Id: CheatCodeDialog.hxx,v 1.2 2005-11-26 21:23:35 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#ifndef CHEAT_CODE_DIALOG_HXX
#define CHEAT_CODE_DIALOG_HXX

class DialogContainer;
class CommandSender;
class ButtonWidget;
class StaticTextWidget;
class CheckListWidget;

#include "OSystem.hxx"
#include "Dialog.hxx"
#include "Widget.hxx"
#include "CheatManager.hxx"
#include "EditTextWidget.hxx"
#include "Props.hxx"
#include "bspf.hxx"

class CheatCodeDialog : public Dialog
{
  public:
    CheatCodeDialog(OSystem* osystem, DialogContainer* parent,
                   int x, int y, int w, int h);
    ~CheatCodeDialog();

  protected:
    virtual void handleCommand(CommandSender* sender, int cmd, int data, int id);
    void loadConfig();
    void saveConfig();

  private:
    void addCheat();
    void editCheat(int cheatNumber);
    void removeCheat(int cheatNumber);

  private:
    CheckListWidget*  myCheatList;
/*
    ButtonWidget*     myExitButton;
    StaticTextWidget* myTitle;
    StaticTextWidget* myError;
    EditTextWidget*   myInput;

    bool myErrorFlag;
*/
};

#endif
