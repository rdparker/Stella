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
// Copyright (c) 1995-2015 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#ifndef CONTROLLER_WIDGET_HXX
#define CONTROLLER_WIDGET_HXX

class GuiObject;
class ButtonWidget;

#include "Widget.hxx"
#include "Command.hxx"

class ControllerWidget : public Widget, public CommandSender
{
  public:
    ControllerWidget(GuiObject* boss, const GUI::Font& font, int x, int y,
                     Controller& controller)
      : Widget(boss, font, x, y, 16, 16),
        CommandSender(boss),
        myController(controller)
    {
      _w = 18 * font.getMaxCharWidth();
      _h = 8 * font.getLineHeight();
    }

    virtual ~ControllerWidget() { }

    virtual void loadConfig() override { }

  protected:
    Controller& myController;

  private:
    virtual void handleCommand(CommandSender* sender, int cmd, int data, int id) override { }

    // Following constructors and assignment operators not supported
    ControllerWidget() = delete;
    ControllerWidget(const ControllerWidget&) = delete;
    ControllerWidget(ControllerWidget&&) = delete;
    ControllerWidget& operator=(const ControllerWidget&) = delete;
    ControllerWidget& operator=(ControllerWidget&&) = delete;
};

#endif
