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
// $Id: Settings.cxx,v 1.1 2002-03-21 22:47:00 stephena Exp $
//============================================================================

#include "Settings.hxx"


Settings::Settings()
{
  theUseFullScreenFlag = false;
  theGrabMouseFlag = false;
  theCenterWindowFlag = false;
  theShowInfoFlag = false;
  theHideCursorFlag = false;
  theUsePrivateColormapFlag = false;
  theMultipleSnapShotFlag = true;
  theDesiredVolume = 75;
  theDesiredFrameRate = 60;
  thePaddleMode = 0;
  theAlternateProFile = "";
  theSnapShotDir = "";
  theSnapShotName = "";
  theWindowSize = 0;
  theMaxWindowSize = 0;
  theHeight = 0;
  theWidth = 0;
}

Settings::~Settings()
{
}

bool Settings::handleCommandLineArgs(int argc, char* argv[])
{
  // Make sure we have the correct number of command line arguments
  if(argc == 1)
    return false;

  for(Int32 i = 1; i < (argc - 1); ++i)
  {
    // See which command line switch they're using
    if(string(argv[i]) == "-fps")
    {
      // They're setting the desired frame rate
      Int32 rate = atoi(argv[++i]);
      if((rate < 1) || (rate > 300))
      {
        rate = 60;
      }

      theDesiredFrameRate = rate;
    }
    else if(string(argv[i]) == "-paddle")
    {
      // They're trying to set the paddle emulation mode
      if(string(argv[i + 1]) == "real")
      {
        thePaddleMode = 4;
      }
      else
      {
        // Make paddle mode 0 if incorrect input is given
        thePaddleMode = atoi(argv[i + 1]);
        if((thePaddleMode < 0) || (thePaddleMode > 3))
        {
          thePaddleMode = 0;
        }
      }
      ++i;
    }
    else if(string(argv[i]) == "-owncmap")
    {
      theUsePrivateColormapFlag = true;
    }
    else if(string(argv[i]) == "-fullscreen")
    {
      theUseFullScreenFlag = true;
    }
    else if(string(argv[i]) == "-grabmouse")
    {
      theGrabMouseFlag = true;
    }
    else if(string(argv[i]) == "-hidecursor")
    {
      theHideCursorFlag = true;
    }
    else if(string(argv[i]) == "-center")
    {
      theCenterWindowFlag = true;
    }
    else if(string(argv[i]) == "-showinfo")
    {
      theShowInfoFlag = true;
    }
    else if(string(argv[i]) == "-zoom")
    {
      uInt32 size = atoi(argv[++i]);
      theWindowSize = size;
    }
    else if(string(argv[i]) == "-volume")
    {
      // They're setting the desired volume
      Int32 volume = atoi(argv[++i]);
      if(volume < 0)
        volume = 0;
      if(volume > 100)
        volume = 100;

      theDesiredVolume = volume;
    }
    else if(string(argv[i]) == "-ssdir")
    {
      theSnapShotDir = argv[++i];
    }
    else if(string(argv[i]) == "-ssname")
    {
      theSnapShotName = argv[++i];
    }
    else if(string(argv[i]) == "-sssingle")
    {
      theMultipleSnapShotFlag = false;
    }
    else if(string(argv[i]) == "-pro")
    {
      theAlternateProFile = argv[++i];
    }
    else
    {
      cout << "Undefined option " << argv[i] << endl;
    }
  }

  return true;
}

void Settings::handleRCFile(istream& in)
{
  string line, key, value;
  uInt32 equalPos;

  while(getline(in, line))
  {
    // Strip all whitespace and tabs from the line
    uInt32 garbage;
    while((garbage = line.find(" ")) != string::npos)
      line.erase(garbage, 1);
    while((garbage = line.find("\t")) != string::npos)
      line.erase(garbage, 1);

    // Ignore commented and empty lines
    if((line.length() == 0) || (line[0] == ';'))
      continue;

    // Search for the equal sign and discard the line if its not found
    if((equalPos = line.find("=")) == string::npos)
      continue;

    key   = line.substr(0, equalPos);
    value = line.substr(equalPos + 1, line.length() - key.length() - 1);

    // Check for absent key or value
    if((key.length() == 0) || (value.length() == 0))
      continue;

    // Now set up the options by key
    if(key == "fps")
    {
      // They're setting the desired frame rate
      uInt32 rate = atoi(value.c_str());
      if((rate < 1) || (rate > 300))
      {
        rate = 60;
      }

      theDesiredFrameRate = rate;
    }
    else if(key == "paddle")
    {
      // They're trying to set the paddle emulation mode
      uInt32 pMode;
      if(value == "real")
      {
        thePaddleMode = 4;
      }
      else
      {
        pMode = atoi(value.c_str());
        if((pMode > 0) && (pMode < 4))
          thePaddleMode = pMode;
      }
    }
    else if(key == "owncmap")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theUsePrivateColormapFlag = true;
      else if(option == 0)
        theUsePrivateColormapFlag = false;
    }
    else if(key == "fullscreen")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theUseFullScreenFlag = true;
      else if(option == 0)
        theUseFullScreenFlag = false;
    }
    else if(key == "grabmouse")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theGrabMouseFlag = true;
      else if(option == 0)
        theGrabMouseFlag = false;
    }
    else if(key == "hidecursor")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theHideCursorFlag = true;
      else if(option == 0)
        theHideCursorFlag = false;
    }
    else if(key == "center")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theCenterWindowFlag = true;
      else if(option == 0)
        theCenterWindowFlag = false;
    }
    else if(key == "showinfo")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theShowInfoFlag = true;
      else if(option == 0)
        theShowInfoFlag = false;
    }
    else if(key == "zoom")
    {
      // They're setting the initial window size
      // Don't do bounds checking here, it will be taken care of later
      uInt32 size = atoi(value.c_str());
      theWindowSize = size;
    }
    else if(key == "volume")
    {
      // They're setting the desired volume
      uInt32 volume = atoi(value.c_str());
      if(volume < 0)
        volume = 0;
      if(volume > 100)
        volume = 100;

      theDesiredVolume = volume;
    }
    else if(key == "ssdir")
    {
      theSnapShotDir = value;
    }
    else if(key == "ssname")
    {
      theSnapShotName = value;
    }
    else if(key == "sssingle")
    {
      uInt32 option = atoi(value.c_str());
      if(option == 1)
        theMultipleSnapShotFlag = false;
      else if(option == 0)
        theMultipleSnapShotFlag = true;
    }
  }
}
