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

#ifndef CARTRIDGEEF_WIDGET_HXX
#define CARTRIDGEEF_WIDGET_HXX

class CartridgeEF;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeEFWidget : public CartDebugWidget
{
  public:
    CartridgeEFWidget(GuiObject* boss, const GUI::Font& lfont,
                      const GUI::Font& nfont,
                      int x, int y, int w, int h,
                      CartridgeEF& cart);
    virtual ~CartridgeEFWidget() { }

  private:
    CartridgeEF& myCart;
    PopUpWidget* myBank;

    enum { kBankChanged = 'bkCH' };

  private:
    void loadConfig() override;
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

    string bankState() override;

    // Following constructors and assignment operators not supported
    CartridgeEFWidget() = delete;
    CartridgeEFWidget(const CartridgeEFWidget&) = delete;
    CartridgeEFWidget(CartridgeEFWidget&&) = delete;
    CartridgeEFWidget& operator=(const CartridgeEFWidget&) = delete;
    CartridgeEFWidget& operator=(CartridgeEFWidget&&) = delete;
};

#endif
