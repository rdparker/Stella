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
// $Id: SettingsUNIX.cxx,v 1.1 2003-10-26 19:40:39 stephena Exp $
//============================================================================

#include <cstdlib>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bspf.hxx"
#include "Console.hxx"
#include "EventHandler.hxx"
#include "StellaEvent.hxx"

#include "Settings.hxx"
#include "SettingsUNIX.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SettingsUNIX::SettingsUNIX()
{
  // First set variables that the parent class needs
  myBaseDir = getenv("HOME");
  string stelladir = myBaseDir + "/.stella";

  if(access(stelladir.c_str(), R_OK|W_OK|X_OK) != 0 )
    mkdir(stelladir.c_str(), 0777);

  myStateDir = stelladir + "/state/";
  if(access(myStateDir.c_str(), R_OK|W_OK|X_OK) != 0 )
    mkdir(myStateDir.c_str(), 0777);

  myUserPropertiesFile   = stelladir + "/stella.pro";
  mySystemPropertiesFile = "/etc/stella.pro";
  myUserConfigFile       = stelladir + "/stellarc";
  mySystemConfigFile     = "/etc/stellarc";

  // Set up the names of the input and output config files
  mySettingsOutputFilename = myUserConfigFile;
  if(access(myUserConfigFile.c_str(), R_OK) == 0)
    mySettingsInputFilename = myUserConfigFile;
  else
    mySettingsInputFilename = mySystemConfigFile;

  mySnapshotFile = "";
  myStateFile    = "";

  // Now create UNIX specific settings
#ifdef DISPLAY_OPENGL
  set("opengl", "false");
#endif
  set("fullscreen", "false");
  set("grabmouse", "false");
  set("center", "false");
  set("hidecursor", "false");
  set("accurate", "true");
  set("volume", "-1");
  set("sound", "oss");
  set("joyleft", "0");
  set("joyright", "1");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SettingsUNIX::~SettingsUNIX()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SettingsUNIX::usage(string& message)
{
  cout << endl
    << message << endl
    << endl
    << "Valid options are:" << endl
    << endl
    << "  -framerate  <number>       Display the given number of frames per second\n"
    << "  -zoom       <size>         Makes window be 'size' times normal\n"
    << "  -fullscreen <0|1>          Play the game in fullscreen mode\n"
    << "  -grabmouse  <0|1>          Keeps the mouse in the game window\n"
    << "  -hidecursor <0|1>          Hides the mouse cursor in the game window\n"
    << "  -center     <0|1>          Centers the game window onscreen\n"
    << "  -volume     <number>       Set the volume (0 - 100)\n"
#ifdef DISPLAY_OPENGL
    << "  -opengl     <0|1>          Use OpenGL mode.\n"
#endif
#ifdef HAVE_JOYSTICK
    << "  -paddle     <0|1|2|3|real> Indicates which paddle the mouse should emulate\n"
    << "                             or that real Atari 2600 paddles are being used\n"
    << "  -joyleft    <number>       The joystick number representing the left controller\n"
    << "  -joyright   <number>       The joystick number representing the right controller\n"
#else
    << "  -paddle     <0|1|2|3>      Indicates which paddle the mouse should emulate\n"
#endif
    << "  -altpro     <props file>   Use the given properties file instead of stella.pro\n"
    << "  -showinfo   <0|1>          Shows some game info\n"
    << "  -accurate   <0|1>          Accurate game timing (uses more CPU)\n"
  #ifdef SNAPSHOT_SUPPORT
    << "  -ssdir      <path>         The directory to save snapshot files to\n"
    << "  -ssname     <name>         How to name the snapshot (romname or md5sum)\n"
    << "  -sssingle   <0|1>          Generate single snapshot instead of many\n"
  #endif
    << endl
    << "  -sound      <type>          Type is one of the following:\n"
    << "               0                Disables all sound generation\n"
  #ifdef SOUND_ALSA
    << "               alsa             ALSA version 0.9 driver\n"
  #endif
  #ifdef SOUND_OSS
    << "               oss              Open Sound System driver\n"
  #endif
    << endl
  #ifdef DEVELOPER_SUPPORT
    << " DEVELOPER options (see Stella manual for details)\n"
    << "  -Dformat                    Sets \"Display.Format\"\n"
    << "  -Dxstart                    Sets \"Display.XStart\"\n"
    << "  -Dwidth                     Sets \"Display.Width\"\n"
    << "  -Dystart                    Sets \"Display.YStart\"\n"
    << "  -Dheight                    Sets \"Display.Height\"\n"
    << "  -mergeprops  <0|1>          Merge changed properties into properties file,\n"
    << "                              or save into a separate file\n"
  #endif
    << endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string SettingsUNIX::stateFilename(uInt32 state)
{
  if(!myConsole)
    return "";

  ostringstream buf;
  buf << myStateDir << myConsole->properties().get("Cartridge.MD5")
      << ".st" << state;

  myStateFile = buf.str();
  return myStateFile;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string SettingsUNIX::snapshotFilename()
{
  if(!myConsole)
    return "";

  string filename;
  string path = getString("ssdir");
  string theSnapshotName = getString("ssname");

  if(theSnapshotName == "romname")
    path = path + "/" + myConsole->properties().get("Cartridge.Name");
  else if(theSnapshotName == "md5sum")
    path = path + "/" + myConsole->properties().get("Cartridge.MD5");

  // Replace all spaces in name with underscores
  replace(path.begin(), path.end(), ' ', '_');

  // Check whether we want multiple snapshots created
  if(!getBool("sssingle"))
  {
    // Determine if the file already exists, checking each successive filename
    // until one doesn't exist
    filename = path + ".png";
    if(access(filename.c_str(), F_OK) == 0 )
    {
      ostringstream buf;
      for(uInt32 i = 1; ;++i)
      {
        buf.str("");
        buf << path << "_" << i << ".png";
        if(access(buf.str().c_str(), F_OK) == -1 )
          break;
      }
      filename = buf.str();
    }
  }
  else
    filename = path + ".png";

  mySnapshotFile = filename;
  return mySnapshotFile;
}
