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
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: OSystemGP2X.hxx,v 1.1 2006-01-08 02:28:03 stephena Exp $
// Modified by Alex Zaballa on 2006/01/04 for use on GP2X
//============================================================================

#ifndef OSYSTEM_GP2X_HXX
#define OSYSTEM_GP2X_HXX

#include "bspf.hxx"


/**
  This class defines GP2X system specific settings.
*/
class OSystemGP2X : public OSystem
{
  public:
    /**
      Create a new GP2X-specific operating system object
    */
    OSystemGP2X();

    /**
      Destructor
    */
    virtual ~OSystemGP2X();

  public:
    /**
      This method runs the main loop.  Since different platforms
      may use different timing methods and/or algorithms, this method has
      been abstracted to each platform.
    */
    virtual void mainLoop();

    /**
      This method returns number of ticks in microseconds.

      @return Current time in microseconds.
    */
    virtual uInt32 getTicks();

    /**
      This method queries the dimensions of the screen for this hardware.
    */
    virtual void getScreenDimensions(int& width, int& height);

    /**
      This method determines the default mapping of joystick buttons to
      Stella events for the PSP device.
    */
    void setDefaultJoymap();

    /**
      This method determines the default mapping of joystick axis to
      Stella events for for the PSP device.
    */
    void setDefaultJoyAxisMap();
};

#endif
