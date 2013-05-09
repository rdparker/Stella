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

#include "Cart4A50.hxx"
#include "PopUpWidget.hxx"
#include "Cart4A50Widget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Cartridge4A50Widget::Cartridge4A50Widget(
      GuiObject* boss, const GUI::Font& font,
      int x, int y, int w, int h, Cartridge4A50& cart)
  : CartDebugWidget(boss, font, x, y, w, h),
    myCart(cart)
{
  string info =
    "4A50 cartridge - 128K ROM and 32K RAM, split in various bank configurations\n"
    "Multiple hotspots, see documentation for further details\n"
    "Lower bank region (2K)   : $F000 - $F7FF\n"
    "Middle bank region (1.5K): $F800 - $FDFF\n"
    "High bank region (256B)  : $FE00 - $FEFF\n"
    "Fixed (last 256B of ROM) : $FF00 - $FFFF\n";

  int xpos = 10,
      ypos = addBaseInformation(cart.mySize, "John Payson / Supercat", info) +
               myLineHeight;

  StringMap items16, items32, items128, items256;
  for(uInt32 i = 0; i < 16; ++i)
  {
    const string& b = BSPF_toString(i);
    items16.push_back(b, b);
  }
  items16.push_back("Inactive", "");

  for(uInt32 i = 0; i < 32; ++i)
  {
    const string& b = BSPF_toString(i);
    items32.push_back(b, b);
  }
  items32.push_back("Inactive", "");

  for(uInt32 i = 0; i < 128; ++i)
  {
    const string& b = BSPF_toString(i);
    items128.push_back(b, b);
  }
  items128.push_back("Inactive", "");

  for(uInt32 i = 0; i < 256; ++i)
  {
    const string& b = BSPF_toString(i);
    items256.push_back(b, b);
  }
  items256.push_back("Inactive", "");

  string lowerlabel  = "Set lower 2K region ($F000 - $F7FF): ";
  string middlelabel = "Set middle 1.5K region ($F800 - $FDFF): ";
  string highlabel   = "Set high 256B region ($FE00 - $FEFF): ";
  const int lwidth  = font.getStringWidth(middlelabel),
            fwidth  = font.getStringWidth("Inactive"),
            flwidth = font.getStringWidth("ROM: ");

  // Lower bank/region configuration
  xpos = 10;
  new StaticTextWidget(_boss, _font, xpos, ypos, lwidth,
    myFontHeight, lowerlabel, kTextAlignLeft);
  ypos += myLineHeight + 8;

  xpos += 40;
  myROMLower =
    new PopUpWidget(boss, font, xpos, ypos-2, fwidth, myLineHeight,
                    items32, "ROM: ", flwidth, kROMLowerChanged);
  myROMLower->setTarget(this);
  addFocusWidget(myROMLower);

  xpos += myROMLower->getWidth() + 20;
  myRAMLower =
    new PopUpWidget(boss, font, xpos, ypos-2, fwidth, myLineHeight,
                    items16, "RAM: ", flwidth, kRAMLowerChanged);
  myRAMLower->setTarget(this);
  addFocusWidget(myRAMLower);

  // Middle bank/region configuration
  xpos = 10;  ypos += myLineHeight + 14;
  new StaticTextWidget(_boss, _font, xpos, ypos, lwidth,
    myFontHeight, middlelabel, kTextAlignLeft);
  ypos += myLineHeight + 8;

  xpos += 40;
  myROMMiddle =
    new PopUpWidget(boss, font, xpos, ypos-2, fwidth, myLineHeight,
                    items32, "ROM: ", flwidth, kROMMiddleChanged);
  myROMMiddle->setTarget(this);
  addFocusWidget(myROMMiddle);

  xpos += myROMMiddle->getWidth() + 20;
  myRAMMiddle =
    new PopUpWidget(boss, font, xpos, ypos-2, fwidth, myLineHeight,
                    items16, "RAM: ", flwidth, kRAMMiddleChanged);
  myRAMMiddle->setTarget(this);
  addFocusWidget(myRAMMiddle);

  // High bank/region configuration
  xpos = 10;  ypos += myLineHeight + 14;
  new StaticTextWidget(_boss, _font, xpos, ypos, lwidth,
    myFontHeight, highlabel, kTextAlignLeft);
  ypos += myLineHeight + 8;

  xpos += 40;
  myROMHigh =
    new PopUpWidget(boss, font, xpos, ypos-2, fwidth, myLineHeight,
                    items256, "ROM: ", flwidth, kROMHighChanged);
  myROMHigh->setTarget(this);
  addFocusWidget(myROMHigh);

  xpos += myROMHigh->getWidth() + 20;
  myRAMHigh =
    new PopUpWidget(boss, font, xpos, ypos-2, fwidth, myLineHeight,
                    items128, "RAM: ", flwidth, kRAMHighChanged);
  myRAMHigh->setTarget(this);
  addFocusWidget(myRAMHigh);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge4A50Widget::loadConfig()
{
  // Lower bank
  if(myCart.myIsRomLow) // ROM active
  {
    myROMLower->setSelected((myCart.mySliceLow >> 11) & 0x1F);
    myRAMLower->setSelectedMax();
  }
  else                  // RAM active
  {
    myROMLower->setSelectedMax();
    myRAMLower->setSelected((myCart.mySliceLow >> 11) & 0x0F);
  }

  // Middle bank
  if(myCart.myIsRomMiddle)  // ROM active
  {
    myROMMiddle->setSelected((myCart.mySliceMiddle >> 11) & 0x1F);
    myRAMMiddle->setSelectedMax();
  }
  else                      // RAM active
  {
    myROMMiddle->setSelectedMax();
    myRAMMiddle->setSelected((myCart.mySliceMiddle >> 11) & 0x0F);
  }

  // High bank
  if(myCart.myIsRomHigh)   // ROM active
  {
    myROMHigh->setSelected((myCart.mySliceHigh >> 11) & 0xFF);
    myRAMHigh->setSelectedMax();
  }
  else                      // RAM active
  {
    myROMHigh->setSelectedMax();
    myRAMHigh->setSelected((myCart.mySliceHigh >> 11) & 0x7F);
  }

  CartDebugWidget::loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge4A50Widget::handleCommand(CommandSender* sender,
                                      int cmd, int data, int id)
{
  myCart.unlockBank();

  switch(cmd)
  {
    case kROMLowerChanged:
      if(myROMLower->getSelected() < 32)
      {
        myCart.bankROMLower(myROMLower->getSelected());
        myRAMLower->setSelectedMax();
      }
      else
      {
        // default to first RAM bank
        myRAMLower->setSelected(0);
        myCart.bankRAMLower(0);
      }
      break;

    case kRAMLowerChanged:
      if(myRAMLower->getSelected() < 16)
      {
        myROMLower->setSelectedMax();
        myCart.bankRAMLower(myRAMLower->getSelected());
      }
      else
      {
        // default to first ROM bank
        myROMLower->setSelected(0);
        myCart.bankROMLower(0);
      }
      break;

    case kROMMiddleChanged:
      if(myROMMiddle->getSelected() < 32)
      {
        myCart.bankROMMiddle(myROMMiddle->getSelected());
        myRAMMiddle->setSelectedMax();
      }
      else
      {
        // default to first RAM bank
        myRAMMiddle->setSelected(0);
        myCart.bankRAMMiddle(0);
      }
      break;

    case kRAMMiddleChanged:
      if(myRAMMiddle->getSelected() < 16)
      {
        myROMMiddle->setSelectedMax();
        myCart.bankRAMMiddle(myRAMMiddle->getSelected());
      }
      else
      {
        // default to first ROM bank
        myROMMiddle->setSelected(0);
        myCart.bankROMMiddle(0);
      }
      break;

    case kROMHighChanged:
      if(myROMHigh->getSelected() < 256)
      {
        myCart.bankROMHigh(myROMHigh->getSelected());
        myRAMHigh->setSelectedMax();
      }
      else
      {
        // default to first RAM bank
        myRAMHigh->setSelected(0);
        myCart.bankRAMHigh(0);
      }
      break;

    case kRAMHighChanged:
      if(myRAMHigh->getSelected() < 128)
      {
        myROMHigh->setSelectedMax();
        myCart.bankRAMHigh(myRAMHigh->getSelected());
      }
      else
      {
        // default to first ROM bank
        myROMHigh->setSelected(0);
        myCart.bankROMHigh(0);
      }
      break;
  }

  myCart.lockBank();
  invalidate();
}