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

#include "Cart0840.hxx"
#include "PopUpWidget.hxx"
#include "Cart0840Widget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Cartridge0840Widget::Cartridge0840Widget(
      GuiObject* boss, const GUI::Font& lfont, const GUI::Font& nfont,
      int x, int y, int w, int h, Cartridge0840& cart)
  : CartDebugWidget(boss, lfont, nfont, x, y, w, h),
    myCart(cart)
{
  uInt16 size = 2 * 4096;

  ostringstream info;
  info << "0840 ECONObanking, two 4K banks\n"
       << "Startup bank = " << cart.myStartBank << "\n";

  // Eventually, we should query this from the debugger/disassembler
  for(uInt32 i = 0, offset = 0xFFC, spot = 0x800; i < 2;
      ++i, offset += 0x1000, spot += 0x40)
  {
    uInt16 start = (cart.myImage[offset+1] << 8) | cart.myImage[offset];
    start -= start % 0x1000;
    info << "Bank " << i << " @ $" << Common::Base::HEX4 << start << " - "
         << "$" << (start + 0xFFF) << " (hotspot = $" << spot << ")\n";
  }

  int xpos = 10,
      ypos = addBaseInformation(size, "Fred X. Quimby", info.str()) + myLineHeight;

  VariantList items;
  VarList::push_back(items, "0 ($800)");
  VarList::push_back(items, "1 ($840)");
  myBank =
    new PopUpWidget(boss, _font, xpos, ypos-2, _font.getStringWidth("0 ($800) "),
                    myLineHeight, items, "Set bank: ",
                    _font.getStringWidth("Set bank: "), kBankChanged);
  myBank->setTarget(this);
  addFocusWidget(myBank);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge0840Widget::loadConfig()
{
  myBank->setSelectedIndex(myCart.myCurrentBank);

  CartDebugWidget::loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Cartridge0840Widget::handleCommand(CommandSender* sender,
                                      int cmd, int data, int id)
{
  if(cmd == kBankChanged)
  {
    myCart.unlockBank();
    myCart.bank(myBank->getSelected());
    myCart.lockBank();
    invalidate();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Cartridge0840Widget::bankState()
{
  ostringstream& buf = buffer();

  static const char* spot[] = { "$800", "$840" };
  buf << "Bank = " << dec << myCart.myCurrentBank
      << ", hotspot = " << spot[myCart.myCurrentBank];

  return buf.str();
}
