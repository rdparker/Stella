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
// Copyright (c) 1995-2014 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#ifndef CARTRIDGEF8SC_WIDGET_HXX
#define CARTRIDGEF8SC_WIDGET_HXX

class CartridgeF8SC;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeF8SCWidget : public CartDebugWidget
{
  public:
    CartridgeF8SCWidget(GuiObject* boss, const GUI::Font& lfont,
                        const GUI::Font& nfont,
                        int x, int y, int w, int h,
                        CartridgeF8SC& cart);
    virtual ~CartridgeF8SCWidget() { }
  
    void saveOldState();
    void loadConfig();
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

    string bankState();
  
    // start of functions for Cartridge RAM tab
    bool internalRam();
    uInt32 internalRamSize();
    string internalRamDescription(); 
    ByteArray internalRamOld(int start, int count);
    ByteArray internalRamCurrent(int start, int count);
    void internalRamSetValue(int addr, uInt8 value);
    uInt8 internalRamGetValue(int addr);
    string internalRamLabel(int addr);
    // end of functions for Cartridge RAM tab


  private:
    CartridgeF8SC& myCart;
    PopUpWidget* myBank;
  
    struct CartState {
      ByteArray internalram;
    };  
    CartState myOldState;

    enum { kBankChanged = 'bkCH' };
};

#endif
