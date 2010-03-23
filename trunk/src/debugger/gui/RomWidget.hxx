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

#ifndef ROM_WIDGET_HXX
#define ROM_WIDGET_HXX

class GuiObject;
class EditTextWidget;
class InputTextDialog;
class PopUpWidget;
class RomListWidget;
class StringList;

#include "Array.hxx"
#include "Widget.hxx"
#include "Command.hxx"
#include "CartDebug.hxx"

class RomWidget : public Widget, public CommandSender
{
  public:
    RomWidget(GuiObject* boss, const GUI::Font& font, int x, int y);
    virtual ~RomWidget();

    void invalidate() { myListIsDirty = true; loadConfig(); }

    void handleCommand(CommandSender* sender, int cmd, int data, int id);
    void loadConfig();

  private:
    void setBreak(int disasm_line, bool state);
    void setPC(int disasm_line);
    void patchROM(int disasm_line, const string& bytes);
    void saveROM(const string& rom);

  private:
    enum {
      kAutocodeChanged = 'ACch',
      kRomNameEntered  = 'RWrn'
    };

    RomListWidget*   myRomList;
    EditTextWidget*  myBank;
    EditTextWidget*  myBankCount;
    PopUpWidget*     myAutocode;
    InputTextDialog* mySaveRom;

    bool myListIsDirty;
    int  myCurrentBank;
};

#endif
