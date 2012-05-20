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

#include <cassert>
#include <iostream>

#include "Console.hxx"
#include "Settings.hxx"
#include "Switches.hxx"
#include "System.hxx"

#include "M6532.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6532::M6532(const Console& console, const Settings& settings)
  : myConsole(console),
    mySettings(settings)
{
}
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6532::~M6532()
{
}
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6532::reset()
{
  // Initialize the 128 bytes of memory
  if(mySettings.getBool("ramrandom"))
    for(uInt32 t = 0; t < 128; ++t)
      myRAM[t] = mySystem->randGenerator().next();
  else
    memset(myRAM, 0, 128);

  // The timer absolutely cannot be initialized to zero; some games will
  // loop or hang (notably Solaris and H.E.R.O.)
  myTimer = (0xff - (mySystem->randGenerator().next() % 0xfe)) << 10;
  myIntervalShift = 10;
  myCyclesWhenTimerSet = 0;
  myInterruptEnabled = false;
  myInterruptTriggered = false;

  // Zero the I/O registers
  myDDRA = myDDRB = myOutA = myOutB = 0x00;

  // Zero the timer registers
  myOutTimer[0] = myOutTimer[1] = myOutTimer[2] = myOutTimer[3] = 0x00;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6532::systemCyclesReset()
{
  // System cycles are being reset to zero so we need to adjust
  // the cycle count we remembered when the timer was last set
  myCyclesWhenTimerSet -= mySystem->cycles();

  // We should also inform any 'smart' controllers as well
  myConsole.controller(Controller::Left).systemCyclesReset();
  myConsole.controller(Controller::Right).systemCyclesReset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6532::install(System& system)
{
  install(system, *this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6532::install(System& system, Device& device)
{
  // Remember which system I'm installed in
  mySystem = &system;

  uInt16 shift = mySystem->pageShift();
  uInt16 mask = mySystem->pageMask();

  // Make sure the system we're being installed in has a page size that'll work
  assert((0x1080 & mask) == 0);
  
  // All accesses are to the given device
  System::PageAccess access(0, 0, 0, &device, System::PA_READWRITE);

  // We're installing in a 2600 system
  for(int address = 0; address < 8192; address += (1 << shift))
    if((address & 0x1080) == 0x0080)
      mySystem->setPageAccess(address >> shift, access);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt8 M6532::peek(uInt16 addr)
{
  // Access RAM directly.  Originally, accesses to RAM could bypass
  // this method and its pages could be installed directly into the
  // system.  However, certain cartridges (notably 4A50) can mirror
  // the RAM address space, making it necessary to chain accesses.
  if((addr & 0x1080) == 0x0080 && (addr & 0x0200) == 0x0000)
  {
    return myRAM[addr & 0x007f];
  }

  switch(addr & 0x07)
  {
    case 0x00:    // SWCHA - Port A I/O Register (Joystick)
    {
      uInt8 value = (myConsole.controller(Controller::Left).read() << 4) |
                     myConsole.controller(Controller::Right).read();

      // Each pin is high (1) by default and will only go low (0) if either
      //  (a) External device drives the pin low
      //  (b) Corresponding bit in SWACNT = 1 and SWCHA = 0
      // Thanks to A. Herbert for this info
      return (myOutA | ~myDDRA) & value;
    }

    case 0x01:    // SWACNT - Port A Data Direction Register 
    {
      return myDDRA;
    }

    case 0x02:    // SWCHB - Port B I/O Register (Console switches)
    {
      return (myOutB | ~myDDRB) & (myConsole.switches().read() | myDDRB);
    }

    case 0x03:    // SWBCNT - Port B Data Direction Register
    {
      return myDDRB;
    }

    case 0x04:    // Timer Output
    case 0x06:
    {
      myInterruptTriggered = false;
      Int32 timer = timerClocks();

      // See if the timer has expired yet?
      // Note that this constant comes from z26, and corresponds to
      // 256 intervals of T1024T (ie, the maximum that the timer should hold)
      // I'm not sure why this is required, but quite a few PAL ROMs fail
      // if we just check >= 0.
      if(!(timer & 0x40000))
      {
        return (timer >> myIntervalShift) & 0xff;
      }
      else
      {
        if(timer != -1)
          myInterruptTriggered = true;

        // According to the M6532 documentation, the timer continues to count
        // down to -255 timer clocks after wraparound.  However, it isn't
        // entirely clear what happens *after* if reaches -255.
        // For now, we'll let it continuously wrap around.
        return timer & 0xff;
      }
    }

    case 0x05:    // Interrupt Flag
    case 0x07:
    {
      if((timerClocks() >= 0) || (myInterruptEnabled && myInterruptTriggered))
        return 0x00;
      else
        return 0x80;
    }

    default:
    {    
#ifdef DEBUG_ACCESSES
      cerr << "BAD M6532 Peek: " << hex << addr << endl;
#endif
      return 0;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6532::poke(uInt16 addr, uInt8 value)
{
  // Access RAM directly.  Originally, accesses to RAM could bypass
  // this method and its pages could be installed directly into the
  // system.  However, certain cartridges (notably 4A50) can mirror
  // the RAM address space, making it necessary to chain accesses.
  if((addr & 0x1080) == 0x0080 && (addr & 0x0200) == 0x0000)
  {
    myRAM[addr & 0x007f] = value;
    return true;
  }

  // A2 distinguishes I/O registers from the timer
  if((addr & 0x04) != 0)
  {
    if((addr & 0x10) != 0)
    {
      myInterruptEnabled = (addr & 0x08);
      setTimerRegister(value, addr & 0x03);
    }
  }
  else
  {
    switch(addr & 0x03)
    {
      case 0:     // SWCHA - Port A I/O Register (Joystick)
      {
        myOutA = value;
        setPinState(true);
        break;
      }

      case 1:     // SWACNT - Port A Data Direction Register 
      {
        myDDRA = value;
        setPinState(false);
        break;
      }

      case 2:     // SWCHB - Port B I/O Register (Console switches)
      {
        myOutB = value;
        break;
      }

      case 3:     // SWBCNT - Port B Data Direction Register 
      {
        myDDRB = value;
        break;
      }
    }
  }
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6532::setTimerRegister(uInt8 value, uInt8 interval)
{
  static const uInt8 shift[] = { 0, 3, 6, 10 };

  myInterruptTriggered = false;
  myIntervalShift = shift[interval];
  myOutTimer[interval] = value;
  myTimer = value << myIntervalShift;
  myCyclesWhenTimerSet = mySystem->cycles();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6532::setPinState(bool swcha)
{
  /*
    When a bit in the DDR is set as input, +5V is placed on its output
    pin.  When it's set as output, either +5V or 0V (depending on the
    contents of SWCHA) will be placed on the output pin.
    The standard macros for the AtariVox and SaveKey use this fact to
    send data to the port.  This is represented by the following algorithm:

      if(DDR bit is input)       set output as 1
      else if(DDR bit is output) set output as bit in ORA
  */
  Controller& port0 = myConsole.controller(Controller::Left);
  Controller& port1 = myConsole.controller(Controller::Right);

  uInt8 ioport = myOutA | ~myDDRA;

  port0.write(Controller::One,   ioport & 0x10);
  port0.write(Controller::Two,   ioport & 0x20);
  port0.write(Controller::Three, ioport & 0x40);
  port0.write(Controller::Four,  ioport & 0x80);
  port1.write(Controller::One,   ioport & 0x01);
  port1.write(Controller::Two,   ioport & 0x02);
  port1.write(Controller::Three, ioport & 0x04);
  port1.write(Controller::Four,  ioport & 0x08);

  if(swcha)
  {
    port0.controlWrite(ioport);
    port1.controlWrite(ioport);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6532::save(Serializer& out) const
{
  try
  {
    out.putString(name());

    // Output the RAM
    out.putByteArray(myRAM, 128);

    out.putInt(myTimer);
    out.putInt(myIntervalShift);
    out.putInt(myCyclesWhenTimerSet);
    out.putBool(myInterruptEnabled);
    out.putBool(myInterruptTriggered);

    out.putByte(myDDRA);
    out.putByte(myDDRB);
    out.putByte(myOutA);
    out.putByte(myOutB);
    out.putByteArray(myOutTimer, 4);
  }
  catch(const char* msg)
  {
    cerr << "ERROR: M6532::save" << endl << "  " << msg << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6532::load(Serializer& in)
{
  try
  {
    if(in.getString() != name())
      return false;

    // Input the RAM
    in.getByteArray(myRAM, 128);

    myTimer = in.getInt();
    myIntervalShift = in.getInt();
    myCyclesWhenTimerSet = in.getInt();
    myInterruptEnabled = in.getBool();
    myInterruptTriggered = in.getBool();

    myDDRA = in.getByte();
    myDDRB = in.getByte();
    myOutA = in.getByte();
    myOutB = in.getByte();
    in.getByteArray(myOutTimer, 4);
  }
  catch(const char* msg)
  {
    cerr << "ERROR: M6532::load" << endl << "  " << msg << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6532::M6532(const M6532& c)
  : myConsole(c.myConsole),
    mySettings(c.mySettings)
{
  assert(false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6532& M6532::operator = (const M6532&)
{
  assert(false);
  return *this;
}
