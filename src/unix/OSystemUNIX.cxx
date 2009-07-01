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
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#include <cstdlib>
#include <unistd.h>

#include "bspf.hxx"
#include "OSystem.hxx"
#include "OSystemUNIX.hxx"

#ifdef HAVE_GETTIMEOFDAY
  #include <time.h>
  #include <sys/time.h>
#endif

/**
  Each derived class is responsible for calling the following methods
  in its constructor:

  setBaseDir()
  setConfigFile()

  See OSystem.hxx for a further explanation
*/

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystemUNIX::OSystemUNIX()
  : OSystem()
{
  setBaseDir("~/.stella");
  setConfigFile("~/.stella/stellarc");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystemUNIX::~OSystemUNIX()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt64 OSystemUNIX::getTicks() const
{
#ifdef HAVE_GETTIMEOFDAY
  timeval now;
  gettimeofday(&now, 0);

  return uInt64(now.tv_sec) * 1000000 + now.tv_usec;
#else
  return uInt64(SDL_GetTicks()) * 1000;
#endif
}
