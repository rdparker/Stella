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
// $Id: MT24LC256.hxx,v 1.1 2008-04-13 15:05:58 stephena Exp $
//============================================================================

#ifndef MT24LC256_HXX
#define MT24LC256_HXX

class Controller;

#include "bspf.hxx"

/**
  Emulates a Microchip Technology Inc. 24LC256, a 32KB Serial Electrically
  Erasable PROM accessed using the I2C protocol.  Thanks to J. Payson
  (aka Supercat) for the bulk of this code.

  @author  Stephen Anthony & J. Payson
  @version $Id: MT24LC256.hxx,v 1.1 2008-04-13 15:05:58 stephena Exp $
*/
class MT24LC256
{
  public:
    /**
      Create a new 24LC256 with its data stored in the given file

      @param filename   Data file representing the EEPROM data
      @param controller The controller attached to this device
    */
    MT24LC256(const string& filename, const Controller* controller);
 
    /**
      Destructor
    */
    virtual ~MT24LC256();

  public:
    /** Read boolean data from the SDA line */
    bool readSDA();

    /** Write boolean data to the SDA and SCL lines */
    void writeSDA(bool state);
    void writeSCL(bool state);

  private:
    // I2C access code provided by Supercat
    void jpee_init();
    void jpee_data_start();
    void jpee_data_stop();
    void jpee_clock_fall();
//    int  jpee_logproc(char const *st);
    int  jpee_timercheck(int mode);

  private:
    // The controller attached to this device
    const Controller* myController;

    // The EEPROM data
    uInt8 myData[32768];

    // The file containing the EEPROM data
    string myDataFile;

    // Required for I2C functionality
    int jpee_mdat, jpee_sdat, jpee_mclk;
    int jpee_sizemask, jpee_pagemask, jpee_smallmode, jpee_logmode;
    int jpee_pptr, jpee_state, jpee_nb;
    unsigned int jpee_address, jpee_ad_known;
    uInt8 jpee_packet[70];

  private:
    // Copy constructor isn't supported by this class so make it private
    MT24LC256(const MT24LC256&);
 
    // Assignment operator isn't supported by this class so make it private
    MT24LC256& operator = (const MT24LC256&);
};
#endif

