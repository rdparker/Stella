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
// Copyright (c) 1995-2008 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: AtariVox.cxx,v 1.17 2008-04-29 15:13:15 stephena Exp $
//============================================================================

#ifdef SPEAKJET_EMULATION
  #include "SpeakJet.hxx"
#endif

#include "MT24LC256.hxx"
#include "SerialPort.hxx"
#include "System.hxx"
#include "AtariVox.hxx"

#define DEBUG_ATARIVOX 0

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AtariVox::AtariVox(Jack jack, const Event& event, const System& system,
                   const SerialPort& port, const string& portname,
                   const string& eepromfile)
  : Controller(jack, event, system, Controller::AtariVox),
    mySerialPort((SerialPort*)&port),
    myEEPROM(NULL),
    myShiftCount(0),
    myShiftRegister(0),
    myLastDataWriteCycle(0)
{
#ifndef SPEAKJET_EMULATION
  if(mySerialPort->openPort(portname))
    myAboutString = " (using serial port \'" + portname + "\')";
  else
    myAboutString = " (invalid serial port \'" + portname + "\')";
#else
  mySpeakJet = new SpeakJet();
  myAboutString = " (emulating SpeakJet device)";
#endif

  myEEPROM = new MT24LC256(eepromfile, system);

  myDigitalPinState[One] = myDigitalPinState[Two] =
  myDigitalPinState[Three] = myDigitalPinState[Four] = true;

  myAnalogPinValue[Five] = myAnalogPinValue[Nine] = maximumResistance;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AtariVox::~AtariVox()
{
#ifndef SPEAKJET_EMULATION
  mySerialPort->closePort();
#else
  delete mySpeakJet;
#endif
  delete myEEPROM;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool AtariVox::read(DigitalPin pin)
{
  // We need to override the Controller::read() method, since the timing
  // of the actual read is important for the EEPROM (we can't just read
  // 60 times per second in the ::update() method)
  switch(pin)
  {
    // Pin 2: SpeakJet READY
    case Two:
      // For now, we just assume the device is always ready
      return myDigitalPinState[Two] = true;

    // Pin 3: EEPROM SDA
    //        input data from the 24LC256 EEPROM using the I2C protocol
    case Three:
      return myDigitalPinState[Three] = myEEPROM->readSDA();

    default:
      return Controller::read(pin);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AtariVox::write(DigitalPin pin, bool value)
{
  if(DEBUG_ATARIVOX)
    cerr << "AtariVox: write to SWCHA" << endl;

  // Change the pin state based on value
  switch(pin)
  {
    // Pin 1: SpeakJet DATA
    //        output serial data to the speakjet
    case One:
      clockDataIn(value);
      break;
  
    // Pin 3: EEPROM SDA
    //        output data to the 24LC256 EEPROM using the I2C protocol
    case Three:
      if(DEBUG_ATARIVOX)
        cerr << "AtariVox: value "
             << value
             << " written to SDA line at cycle "
             << mySystem.cycles()
             << endl;
      myEEPROM->writeSDA(value);
      break;

    // Pin 4: EEPROM SCL
    //        output clock data to the 24LC256 EEPROM using the I2C protocol
    case Four:
      if(DEBUG_ATARIVOX)
        cerr << "AtariVox: value "
             << value
             << " written to SCLK line at cycle "
             << mySystem.cycles()
             << endl;
      myEEPROM->writeSCL(value);
      break;

    default:
      break;
  } 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AtariVox::clockDataIn(bool value)
{
  // Data is normally inverted when sending to the SpeakJet;
  // we need to reverse that
//  value = !value;

  uInt32 cycle = mySystem.cycles();
  if(DEBUG_ATARIVOX)
    cerr << "AtariVox: value "
         << value
         << " written to DATA line at "
         << mySystem.cycles()
         << " (-"
         << myLastDataWriteCycle
         << "=="
         << (mySystem.cycles() - myLastDataWriteCycle)
         << ")"
         << endl;

  if(value && (myShiftCount == 0))
  {
    if(DEBUG_ATARIVOX)
      cerr << "value && (myShiftCount == 0), returning" << endl;
cerr << "!!!!! INVALID START BIT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    return;
  }

  // If this is the first write this frame, or if it's been a long time
  // since the last write, start a new data byte.
  if(cycle < myLastDataWriteCycle)
  {
    myShiftRegister = 0;
    myShiftCount = 0;
cerr << "!!!!! START NEW BYTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
  }
  else if(cycle > myLastDataWriteCycle + 1000)
  {
    myShiftRegister = 0;
    myShiftCount = 0;
cerr << "!!!!! DELAY TOO LONG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
  }

cerr << "value = " << value << " (" << (int)myShiftCount << ")" << endl;

  // If this is the first write this frame, or if it's been 62 cycles
  // since the last write, shift this bit into the current byte.
  if(cycle < myLastDataWriteCycle || cycle >= myLastDataWriteCycle + 62)
  {
    if(DEBUG_ATARIVOX)
      cerr << "cycle >= myLastDataWriteCycle + 62, shiftIn("
           << value << ")" << endl;

    myShiftRegister >>= 1;
    myShiftRegister |= (value << 15);
    if(++myShiftCount == 10)
    {
      myShiftCount = 0;
      myShiftRegister >>= 6;
      if(!(myShiftRegister & (1<<9)))
        cerr << "AtariVox: bad start bit" << endl;
      else if((myShiftRegister & 1))
        cerr << "AtariVox: bad stop bit" << endl;
      else
      {
        uInt8 data = ((myShiftRegister >> 1) & 0xff);
    #ifndef SPEAKJET_EMULATION
        mySerialPort->writeByte(&data);
    #else
        mySpeakJet->write(data);
    #endif
cerr << " ==> " << (int)data << endl;
cerr << "----------------------------------------------------------\n";
      }
      myShiftRegister = 0;
    }
  }

  myLastDataWriteCycle = cycle;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string AtariVox::about() const
{
  return Controller::about() + myAboutString;
}
