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
// Copyright (c) 1995-2005 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Menu.hxx,v 1.1 2005-02-27 23:41:19 stephena Exp $
//============================================================================

#ifndef MENU_HXX
#define MENU_HXX

#include "bspf.hxx"

/**
  The base class for all menus in Stella.
  
  This class keeps track of all configuration menus. organizes them into
  a stack, and handles their events.

  @author  Stephen Anthony
  @version $Id: Menu.hxx,v 1.1 2005-02-27 23:41:19 stephena Exp $
*/
class Menu
{
  public:
    /**
      Create a new menu stack
    */
    Menu();

    /**
      Destructor
    */
    virtual ~Menu();

  public:
    /**
      Handle a keyboard event.

      @param key   keysym
      @param mod   modifiers
      @param state state of key
    */
    void handleKeyEvent(SDLKey key, SDLMod mod, uInt8 state);

// FIXME - add mouse and joystick handlers  

    /**
      Draw the stack of menus.
    */
    void draw();

  private:

};

#endif
