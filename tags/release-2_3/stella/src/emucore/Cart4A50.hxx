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
// Copyright (c) 1995-2006 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Cart4A50.hxx,v 1.3 2006-12-08 16:49:20 stephena Exp $
//============================================================================

#ifndef CARTRIDGE4A50_HXX
#define CARTRIDGE4A50_HXX

class Cartridge4A50;
class System;
class Serializer;
class Deserializer;

#include "bspf.hxx"
#include "Cart.hxx"

/**
  This is the standard Atari 4K cartridge.  These cartridges are 
  not bankswitched.

  @author  Bradford W. Mott
  @version $Id: Cart4A50.hxx,v 1.3 2006-12-08 16:49:20 stephena Exp $
*/
class Cartridge4A50 : public Cartridge
{
  public:
    /**
      Create a new cartridge using the specified image

      @param image Pointer to the ROM image
    */
    Cartridge4A50(const uInt8* image);
 
    /**
      Destructor
    */
    virtual ~Cartridge4A50();

  public:
    /**
      Get a null terminated string which is the device's name (i.e. "M6532")

      @return The name of the device
    */
    virtual const char* name() const;

    /**
      Reset cartridge to its power-on state
    */
    virtual void reset();

    /**
      Install cartridge in the specified system.  Invoked by the system
      when the cartridge is attached to it.

      @param system The system the device should install itself in
    */
    virtual void install(System& system);

    /**
      Saves the current state of this device to the given Serializer.

      @param out The serializer device to save to.
      @return The result of the save.  True on success, false on failure.
    */
    virtual bool save(Serializer& out);

    /**
      Loads the current state of this device from the given Deserializer.

      @param in The deserializer device to load from.
      @return The result of the load.  True on success, false on failure.
    */
    virtual bool load(Deserializer& in);

    virtual uInt8* getImage(int& size);

  public:
    /**
      Get the byte at the specified address.

      @return The byte at the specified address
    */
    virtual uInt8 peek(uInt16 address);

    /**
      Change the byte at the specified address to the given value

      @param address The address where the value should be stored
      @param value The value to be stored at the address
    */
    virtual void poke(uInt16 address, uInt8 value);

    bool patch(uInt16 address, uInt8 value);

  private:
    // The 4K ROM image for the cartridge
//    uInt8 myImage[4096];
};

#endif
