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
// Copyright (c) 1995-1999 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: OSystemUNIX.hxx,v 1.3 2005-02-22 18:41:16 stephena Exp $
//============================================================================

#ifndef OSYSTEM_UNIX_HXX
#define OSYSTEM_UNIX_HXX

#include "bspf.hxx"


/**
  This class defines UNIX-like OS's (Linux) system specific settings.

  @author  Stephen Anthony
  @version $Id: OSystemUNIX.hxx,v 1.3 2005-02-22 18:41:16 stephena Exp $
*/
class OSystemUNIX : public OSystem
{
  public:
    /**
      Create a new UNIX-specific operating system object
    */
    OSystemUNIX();

    /**
      Destructor
    */
    virtual ~OSystemUNIX();

  public:
    /**
      This method should be called to get the filename of a state file
      given the state number.

      @param md5   The md5sum to use as part of the filename.
      @param state The state to use as part of the filename.

      @return String representing the full path of the state filename.
    */
    virtual string stateFilename(const string& md5, uInt32 state);


    /**
      This method should be called to test whether the given file exists.

      @param filename The filename to test for existence.

      @return boolean representing whether or not the file exists
    */
    virtual bool fileExists(const string& filename);

    /**
      This method should be called to create the specified directory.

      @param path   The directory to create

      @return       boolean representing whether or not the directory was created
    */
    virtual bool makeDir(const string& path);
};

#endif
