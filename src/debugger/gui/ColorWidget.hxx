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

#ifndef COLOR_WIDGET_HXX
#define COLOR_WIDGET_HXX

class ColorDialog;
class GuiObject;

#include "Widget.hxx"
#include "Command.hxx"

/**
  Displays a color from the TIA palette.  This class will eventually
  be expanded with a TIA palette table, to set the color visually.
  
  @author  Stephen Anthony
  @version $Id$
*/
class ColorWidget : public Widget, public CommandSender
{
  friend class ColorDialog;

  public:
    ColorWidget(GuiObject* boss, const GUI::Font& font,
                int x, int y, int w, int h, int cmd = 0);
    ~ColorWidget();

    void setColor(int color);
    int  getColor() const { return _color;  }
	
  protected:
    void drawWidget(bool hilite) override;

  protected:
    int _color;
    int	_cmd;

  private:
    // Following constructors and assignment operators not supported
    ColorWidget() = delete;
    ColorWidget(const ColorWidget&) = delete;
    ColorWidget(ColorWidget&&) = delete;
    ColorWidget& operator=(const ColorWidget&) = delete;
    ColorWidget& operator=(ColorWidget&&) = delete;
};

#endif
