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
// $Id: Settings.hxx,v 1.2 2003-09-07 18:30:28 stephena Exp $
//============================================================================

#ifndef SETTINGS_HXX
#define SETTINGS_HXX

#include "bspf.hxx"

#ifdef DEVELOPER_SUPPORT
  #include "Props.hxx"
#endif

class Console;

class Settings
{
  public:
    Settings(const string& infile, const string& outfile);
    ~Settings();

    bool handleCommandLineArgs(int ac, char* av[]);
    void save();
    void setConsole(Console* console);

  public:
    // The keymap to use
    string theKeymapList;

    // The joymap to use
    string theJoymapList;

    // Indicates whether to use fullscreen
    bool theUseFullScreenFlag;

    // Indicates whether mouse can leave the game window
    bool theGrabMouseFlag;

    // Indicates whether to center the game window
    bool theCenterWindowFlag;

    // Indicates whether to show some game info on program exit
    bool theShowInfoFlag;

    // Indicates whether to show cursor in the game window
    bool theHideCursorFlag;

    // Indicates whether to allocate colors from a private color map
    bool theUsePrivateColormapFlag;

    // Indicates whether to generate multiple snapshots or keep
    // overwriting the same file.  Set to true by default.
    bool theMultipleSnapShotFlag;

    // Indicates whether to use more/less accurate emulation,
    // resulting in more/less CPU usage.
    bool theAccurateTimingFlag;

    // Indicates what the desired volume is
    Int32 theDesiredVolume;

    // Indicates what the desired frame rate is
    uInt32 theDesiredFrameRate;

    // Indicate which paddle mode we're using:
    //   0 - Mouse emulates paddle 0
    //   1 - Mouse emulates paddle 1
    //   2 - Mouse emulates paddle 2
    //   3 - Mouse emulates paddle 3
    //   4 - Use real Atari 2600 paddles
    uInt32 thePaddleMode;

    // An alternate properties file to use
    string theAlternateProFile;

    // The path to save snapshot files
    string theSnapShotDir;

    // What the snapshot should be called (romname or md5sum)
    string theSnapShotName;

    // Indicates which sound driver to use at run-time
    string theSoundDriver;

    // The size of the window/screen
    uInt32 theWindowSize;

#ifdef DEVELOPER_SUPPORT
    // User-modified properties
    Properties userDefinedProperties;

    // Whether to save user-defined properties to a file or
    // merge into the propertiesset file for future use
    bool theMergePropertiesFlag;
#endif

  private:
    void handleRCFile();

    // The full pathname of the settings file for input
    string mySettingsInputFilename;

    // The full pathname of the settings file for output
    string mySettingsOutputFilename;

    // The global Console object
    Console* myConsole;
};

#endif
