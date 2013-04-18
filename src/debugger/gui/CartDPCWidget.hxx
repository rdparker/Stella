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
// Copyright (c) 1995-2013 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#ifndef CARTRIDGEDPC_WIDGET_HXX
#define CARTRIDGEDPC_WIDGET_HXX

class CartridgeDPC;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeDPCWidget : public CartDebugWidget
{
  public:
    CartridgeDPCWidget(GuiObject* boss, const GUI::Font& font,
                       int x, int y, int w, int h,
                       CartridgeDPC& cart);
    virtual ~CartridgeDPCWidget() { }

    void loadConfig();
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

  private:
    CartridgeDPC& myCart;
    PopUpWidget* myBank;

    enum { kBankChanged = 'bkCH' };
};

#endif
