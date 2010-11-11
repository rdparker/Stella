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
// $Id: InputDialog.hxx,v 1.8 2006-04-05 12:28:39 stephena Exp $
//============================================================================

#ifndef INPUT_DIALOG_HXX
#define INPUT_DIALOG_HXX

class OSystem;
class GuiObject;
class TabWidget;
class EventMappingWidget;
class CheckBoxWidget;
class PopUpWidget;
class SliderWidget;
class StaticTextWidget;

#include "Dialog.hxx"
#include "bspf.hxx"

class InputDialog : public Dialog
{
  public:
    InputDialog(OSystem* osystem, DialogContainer* parent,
                const GUI::Font& font, int x, int y, int w, int h);
    ~InputDialog();

  protected:
    virtual void handleKeyDown(int ascii, int keycode, int modifiers);
    virtual void handleJoyDown(int stick, int button);
    virtual void handleJoyAxis(int stick, int axis, int value);
    virtual bool handleJoyHat(int stick, int hat, int value);
    virtual void handleCommand(CommandSender* sender, int cmd, int data, int id);

    void loadConfig();
    void saveConfig();

  private:
    void addVDeviceTab(const GUI::Font& font);

  private:
    TabWidget* myTab;

    EventMappingWidget* myEventMapper;

    PopUpWidget* myLeftPort;
    PopUpWidget* myRightPort;

    SliderWidget*     myPaddleMode;
    StaticTextWidget* myPaddleModeLabel;
    SliderWidget*     myPaddleThreshold;
    StaticTextWidget* myPaddleThresholdLabel;
    SliderWidget*     myPaddleSpeed[4];
    StaticTextWidget* myPaddleLabel[4];

    CheckBoxWidget* myJoyMouse;
};

#endif