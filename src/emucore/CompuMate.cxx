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
// Copyright (c) 1995-2012 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#include "Control.hxx"
#include "System.hxx"
#include "StellaKeys.hxx"
#include "CompuMate.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompuMate::CompuMate(CartridgeCM& cart, const Event& event,
                     const System& system)
  : myCart(cart),
    myEvent(event),
    mySystem(system),
    myLeftController(0),
    myRightController(0),
    myCycleAtLastUpdate(0)
{
  myLeftController = new CMControl(*this, Controller::Left, event, system);
  myRightController = new CMControl(*this, Controller::Right, event, system);

  myLeftController->myAnalogPinValue[Controller::Nine] = Controller::maximumResistance;
  myLeftController->myAnalogPinValue[Controller::Five] = Controller::minimumResistance;
  myRightController->myAnalogPinValue[Controller::Nine] = Controller::minimumResistance;
  myRightController->myAnalogPinValue[Controller::Five] = Controller::maximumResistance;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CompuMate::update()
{
  uInt32 cycle = mySystem.cycles();

  // Only perform update once for both ports in the same cycle
  if(myCycleAtLastUpdate != cycle)
  {
    myCycleAtLastUpdate = cycle;
    return;
  }
  myCycleAtLastUpdate = cycle;

  // Handle SWCHA changes - the following comes almost directly from z26
  Controller& lp = *myLeftController;
  Controller& rp = *myRightController;
  uInt8* KeyTable = StellaKeys::GetKeyState();

  lp.myAnalogPinValue[Controller::Nine] = Controller::maximumResistance;
  lp.myAnalogPinValue[Controller::Five] = Controller::minimumResistance;
  lp.myDigitalPinState[Controller::Six] = true;
  rp.myAnalogPinValue[Controller::Nine] = Controller::minimumResistance;
  rp.myAnalogPinValue[Controller::Five] = Controller::maximumResistance;
  rp.myDigitalPinState[Controller::Six] = true;
  rp.myDigitalPinState[Controller::Three] = true;
  rp.myDigitalPinState[Controller::Four] = true;

  if (KeyTable[KBDK_LSHIFT] || KeyTable[KBDK_RSHIFT])
    rp.myAnalogPinValue[Controller::Five] = Controller::minimumResistance;
  if (KeyTable[KBDK_LCTRL] || KeyTable[KBDK_RCTRL])
    lp.myAnalogPinValue[Controller::Nine] = Controller::minimumResistance;

  switch(myCart.column())
  {
    case 0:
      if (KeyTable[KBDK_7]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_u]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_j]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_m]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 1:
      if (KeyTable[KBDK_6]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_y]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_h]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_n]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 2:
      if (KeyTable[KBDK_8]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_i]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_k]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_COMMA]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 3:
      if (KeyTable[KBDK_2]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_w]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_s]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_x]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 4:
      if (KeyTable[KBDK_3]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_e]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_d]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_c]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 5:
      if (KeyTable[KBDK_0]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_p]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_SEMICOLON] || KeyTable[KBDK_RETURN])
                            rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_SLASH] || KeyTable[KBDK_SPACE])
                            rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 6:
      if (KeyTable[KBDK_9]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_o]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_l]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_PERIOD]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 7:
      if (KeyTable[KBDK_5]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_t]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_g]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_b]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 8:
      if (KeyTable[KBDK_1]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_q]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_a]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_z]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    case 9:
      if (KeyTable[KBDK_4]) lp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_r]) rp.myDigitalPinState[Controller::Three] = false;
      if (KeyTable[KBDK_f]) rp.myDigitalPinState[Controller::Six] = false;
      if (KeyTable[KBDK_v]) rp.myDigitalPinState[Controller::Four] = false;
      break;
    default:
      break;
  }
}
