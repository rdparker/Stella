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
// $Id: CartF6SC.hxx,v 1.3 2005-02-13 19:17:02 stephena Exp $
//============================================================================

#ifndef CARTRIDGEF6SC_HXX
#define CARTRIDGEF6SC_HXX

class CartridgeF6SC;
class Serializer;
class Deserializer;

#include "bspf.hxx"
#include "Cart.hxx"

/**
  Cartridge class used for Atari's 16K bankswitched games with
  128 bytes of RAM.  There are four 4K banks.

  @author  Bradford W. Mott
  @version $Id: CartF6SC.hxx,v 1.3 2005-02-13 19:17:02 stephena Exp $
*/
class CartridgeF6SC : public Cartridge
{
  public:
    /**
      Create a new cartridge using the specified image

      @param image Pointer to the ROM image
    */
    CartridgeF6SC(const uInt8* image);
 
    /**
      Destructor
    */
    virtual ~CartridgeF6SC();

  public:
    /**
      Get a null terminated string which is the device's name (i.e. "M6532")

      @return The name of the device
    */
    virtual const char* name() const;

    /**
      Reset device to its power-on state
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

  private:
    /**
      Install pages for the specified bank in the system

      @param bank The bank that should be installed in the system
    */
    void bank(uInt16 bank);

  private:
    // Indicates which bank is currently active
    uInt16 myCurrentBank;

    // The 16K ROM image of the cartridge
    uInt8 myImage[16384];

    // The 128 bytes of RAM
    uInt8 myRAM[128];
};
#endif
