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

#ifndef CARTRIDGEFA2_WIDGET_HXX
#define CARTRIDGEFA2_WIDGET_HXX

class CartridgeFA2;
class ButtonWidget;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeFA2Widget : public CartDebugWidget
{
  public:
    CartridgeFA2Widget(GuiObject* boss, const GUI::Font& lfont,
                       const GUI::Font& nfont,
                       int x, int y, int w, int h,
                       CartridgeFA2& cart);
    virtual ~CartridgeFA2Widget() { }

  private:
    CartridgeFA2& myCart;
    PopUpWidget* myBank;
    ButtonWidget *myFlashErase, *myFlashLoad, *myFlashSave;
  
    struct CartState {
      ByteArray internalram;
    };  
    CartState myOldState;   

    enum {
      kBankChanged = 'bkCH',
      kFlashErase  = 'flER',
      kFlashLoad   = 'flLD',
      kFlashSave   = 'flSV'
    };

  private:
    void saveOldState() override;
    void loadConfig() override;
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

    string bankState() override;
  
    // start of functions for Cartridge RAM tab
    uInt32 internalRamSize() override;
    uInt32 internalRamRPort(int start) override;
    string internalRamDescription() override;
    const ByteArray& internalRamOld(int start, int count) override;
    const ByteArray& internalRamCurrent(int start, int count) override;
    void internalRamSetValue(int addr, uInt8 value) override;
    uInt8 internalRamGetValue(int addr) override;
    string internalRamLabel(int addr) override;
    // end of functions for Cartridge RAM tab

    // Following constructors and assignment operators not supported
    CartridgeFA2Widget() = delete;
    CartridgeFA2Widget(const CartridgeFA2Widget&) = delete;
    CartridgeFA2Widget(CartridgeFA2Widget&&) = delete;
    CartridgeFA2Widget& operator=(const CartridgeFA2Widget&) = delete;
    CartridgeFA2Widget& operator=(CartridgeFA2Widget&&) = delete;
};

#endif
