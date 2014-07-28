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

#ifndef CARTRIDGEFE_HXX
#define CARTRIDGEFE_HXX

class System;

#include "bspf.hxx"
#include "Cart.hxx"
#ifdef DEBUGGER_SUPPORT
  #include "CartFEWidget.hxx"
#endif

/**
  Bankswitching method used by Activision's Robot Tank and Decathlon.

  Kevin Horton describes FE as follows:

    Used only on two carts (Robot Tank and Decathlon).  These 
    carts are very weird.  It does not use accesses to the stack 
    like was previously thought.  Instead, if you watch the called 
    addresses very carefully, you can see that they are either Dxxx 
    or Fxxx.  This determines the bank to use.  Just monitor A13 of 
    the processor and use it to determine your bank! :-)  Of course 
    the 6507 in the 2600 does not have an A13, so the cart must have 
    an extra bit in the ROM matrix to tell when to switch banks.  
    There is *no* way to determine which bank you want to be in from
    monitoring the bus.

  This cart reports having 2 banks, even though this cannot be
  determined on a real system.

  @author  Bradford W. Mott
  @version $Id$
*/
class CartridgeFE : public Cartridge
{
  friend class CartridgeFEWidget;

  public:
    /**
      Create a new cartridge using the specified image

      @param image     Pointer to the ROM image
      @param size      The size of the ROM image
      @param settings  A reference to the various settings (read-only)
    */
    CartridgeFE(const uInt8* image, uInt32 size, const Settings& settings);
 
    /**
      Destructor
    */
    virtual ~CartridgeFE();

  public:
    /**
      Reset device to its power-on state
    */
    void reset();

    /**
      Install cartridge in the specified system.  Invoked by the system
      when the cartridge is attached to it.

      @param system The system the device should install itself in
    */
    void install(System& system);

    /**
      Get the current bank.
    */
    uInt16 getBank() const;

    /**
      Query the number of banks supported by the cartridge.
    */
    uInt16 bankCount() const;

    /**
      Answer whether the bank has changed since the last time this
      method was called.

      @return  Whether the bank was changed
    */
    bool bankChanged();

    /**
      Patch the cartridge ROM.

      @param address  The ROM address to patch
      @param value    The value to place into the address
      @return    Success or failure of the patch operation
    */
    bool patch(uInt16 address, uInt8 value);

    /**
      Access the internal ROM image for this cartridge.

      @param size  Set to the size of the internal ROM image data
      @return  A pointer to the internal ROM image data
    */
    const uInt8* getImage(int& size) const;

    /**
      Save the current state of this cart to the given Serializer.

      @param out  The Serializer object to use
      @return  False on any errors, else true
    */
    bool save(Serializer& out) const;

    /**
      Load the current state of this cart from the given Serializer.

      @param in  The Serializer object to use
      @return  False on any errors, else true
    */
    bool load(Serializer& in);

    /**
      Get a descriptor for the device name (used in error checking).

      @return The name of the object
    */
    string name() const { return "CartridgeFE"; }

  #ifdef DEBUGGER_SUPPORT
    /**
      Get debugger widget responsible for accessing the inner workings
      of the cart.
    */
    CartDebugWidget* debugWidget(GuiObject* boss, const GUI::Font& lfont,
        const GUI::Font& nfont, int x, int y, int w, int h)
    {
      return new CartridgeFEWidget(boss, lfont, nfont, x, y, w, h, *this);
    }
  #endif

  public:
    /**
      Get the byte at the specified address.

      @return The byte at the specified address
    */
    uInt8 peek(uInt16 address);

    /**
      Change the byte at the specified address to the given value

      @param address The address where the value should be stored
      @param value The value to be stored at the address
      @return  True if the poke changed the device address space, else false
    */
    bool poke(uInt16 address, uInt8 value);

  private:
    /**
      Query/change the given address type to use the given disassembly flags

      @param address The address to modify
      @param flags A bitfield of DisasmType directives for the given address
    */
    uInt8 getAccessFlags(uInt16 address);
    void setAccessFlags(uInt16 address, uInt8 flags);

  private:
    // The 8K ROM image of the cartridge
    uInt8 myImage[8192];

    // Previous two addresses accessed by peek()
    uInt16 myLastAddress1, myLastAddress2;

    // Last two addresses have been modified by peek()
    bool myLastAddressChanged;
};

#endif
