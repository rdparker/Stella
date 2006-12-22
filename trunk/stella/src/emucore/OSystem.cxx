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
// Copyright (c) 1995-2006 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: OSystem.cxx,v 1.83 2006-12-22 23:14:39 stephena Exp $
//============================================================================

#include <cassert>
#include <sstream>
#include <fstream>

#include "MediaFactory.hxx"

#ifdef DEBUGGER_SUPPORT
  #include "Debugger.hxx"
#endif

#ifdef CHEATCODE_SUPPORT
  #include "CheatManager.hxx"
#endif

#include "unzip.h"
#include "MD5.hxx"
#include "FSNode.hxx"
#include "Settings.hxx"
#include "PropsSet.hxx"
#include "EventHandler.hxx"
#include "Menu.hxx"
#include "CommandMenu.hxx"
#include "Launcher.hxx"
#include "Font.hxx"
#include "StellaFont.hxx"
#include "ConsoleFont.hxx"
#include "bspf.hxx"
#include "OSystem.hxx"
#include "Widget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystem::OSystem()
  : myEventHandler(NULL),
    myFrameBuffer(NULL),
    mySound(NULL),
    mySettings(NULL),
    myPropSet(NULL),
    myConsole(NULL),
    myMenu(NULL),
    myCommandMenu(NULL),
    myLauncher(NULL),
    myDebugger(NULL),
    myCheatManager(NULL),
    myRomFile(""),
    myFeatures(""),
    myFont(NULL),
    myConsoleFont(NULL)
{
#if 0
  // Debugging info for the GUI widgets
  cerr << "  kStaticTextWidget   = " << kStaticTextWidget   << endl;
  cerr << "  kEditTextWidget     = " << kEditTextWidget     << endl;
  cerr << "  kButtonWidget       = " << kButtonWidget       << endl;
  cerr << "  kCheckboxWidget     = " << kCheckboxWidget     << endl;
  cerr << "  kSliderWidget       = " << kSliderWidget       << endl;
  cerr << "  kListWidget         = " << kListWidget         << endl;
  cerr << "  kScrollBarWidget    = " << kScrollBarWidget    << endl;
  cerr << "  kPopUpWidget        = " << kPopUpWidget        << endl;
  cerr << "  kTabWidget          = " << kTabWidget	        << endl;
  cerr << "  kEventMappingWidget = " << kEventMappingWidget << endl;
  cerr << "  kEditableWidget     = " << kEditableWidget     << endl;
  cerr << "  kAudioWidget        = " << kAudioWidget        << endl;
  cerr << "  kColorWidget        = " << kColorWidget        << endl;
  cerr << "  kCpuWidget          = " << kCpuWidget          << endl;
  cerr << "  kDataGridOpsWidget  = " << kDataGridOpsWidget  << endl;
  cerr << "  kDataGridWidget     = " << kDataGridWidget     << endl;
  cerr << "  kPromptWidget       = " << kPromptWidget       << endl;
  cerr << "  kRamWidget          = " << kRamWidget          << endl;
  cerr << "  kRomListWidget      = " << kRomListWidget      << endl;
  cerr << "  kRomWidget          = " << kRomWidget          << endl;
  cerr << "  kTiaInfoWidget      = " << kTiaInfoWidget      << endl;
  cerr << "  kTiaOutputWidget    = " << kTiaOutputWidget    << endl;
  cerr << "  kTiaWidget          = " << kTiaWidget          << endl;
  cerr << "  kTiaZoomWidget      = " << kTiaZoomWidget      << endl;
  cerr << "  kToggleBitWidget    = " << kToggleBitWidget    << endl;
  cerr << "  kTogglePixelWidget  = " << kTogglePixelWidget  << endl;
  cerr << "  kToggleWidget       = " << kToggleWidget       << endl;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystem::~OSystem()
{
  delete myMenu;
  delete myCommandMenu;
  delete myLauncher;
  delete myFont;
  delete myConsoleFont;

  // Remove any game console that is currently attached
  delete myConsole;

  // OSystem takes responsibility for framebuffer and sound,
  // since it created them
  delete myFrameBuffer;
  delete mySound;

  // These must be deleted after all the others
  // This is a bit hacky, since it depends on ordering
  // of d'tor calls
#ifdef DEBUGGER_SUPPORT
  delete myDebugger;
#endif
#ifdef CHEATCODE_SUPPORT
  delete myCheatManager;
#endif

  delete myPropSet;
  delete myEventHandler;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool OSystem::create()
{
  // Create fonts to draw text
  myFont         = new GUI::Font(GUI::stellaDesc);
  myLauncherFont = new GUI::Font(GUI::stellaDesc);  // FIXME
  myConsoleFont  = new GUI::Font(GUI::consoleDesc);

  // Create menu and launcher GUI objects
  myMenu = new Menu(this);
  myCommandMenu = new CommandMenu(this);
  myLauncher = new Launcher(this);
#ifdef DEBUGGER_SUPPORT
  myDebugger = new Debugger(this);
#endif
#ifdef CHEATCODE_SUPPORT
  myCheatManager = new CheatManager(this);
  myCheatManager->loadCheatDatabase();
#endif

  // Create the event handler for the system
  myEventHandler = new EventHandler(this);
  myEventHandler->initialize();

  // Create a properties set for us to use and set it up
  myPropSet = new PropertiesSet(this);

  // Create the sound object; the sound subsystem isn't actually
  // opened until needed, so this is non-blocking (on those systems
  // that only have a single sound device (no hardware mixing)
  createSound();

  // Determine which features were conditionally compiled into Stella
#ifdef DISPLAY_OPENGL
  myFeatures += "OpenGL ";
#endif
#ifdef SOUND_SUPPORT
  myFeatures += "Sound ";
#endif
#ifdef JOYSTICK_SUPPORT
  myFeatures += "Joystick ";
#endif
  myFeatures += "Snapshot ";
#ifdef DEBUGGER_SUPPORT
  myFeatures += "Debugger ";
#endif
#ifdef CHEATCODE_SUPPORT
  myFeatures += "Cheats";
#endif

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setBaseDir(const string& basedir)
{
  myBaseDir = basedir;
  if(!FilesystemNode::dirExists(myBaseDir))
    FilesystemNode::makeDir(myBaseDir);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setStateDir(const string& statedir)
{
  myStateDir = statedir;
  if(!FilesystemNode::dirExists(myStateDir))
    FilesystemNode::makeDir(myStateDir);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setPropertiesDir(const string& path)
{
  myPropertiesFile  = path + BSPF_PATH_SEPARATOR + "stella.pro";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setConfigFile(const string& file)
{
  myConfigFile = file;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setFramerate(uInt32 framerate)
{
  myDisplayFrameRate = framerate;
  myTimePerFrame = (uInt32)(1000000.0 / (double)myDisplayFrameRate);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool OSystem::createFrameBuffer(bool showmessage)
{
  // Check if we can re-use the current framebuffer
  bool changeBuffer = (myFrameBuffer == NULL);
  if(!changeBuffer)
  {
    if((mySettings->getString("video") == "soft" &&
        myFrameBuffer->type() != kSoftBuffer) ||
       (mySettings->getString("video") == "gl" &&
        myFrameBuffer->type() != kGLBuffer))
      changeBuffer = true;
  }
  // Now we only create when absolutely necessary
  if(changeBuffer)
  {
    delete myFrameBuffer;
    myFrameBuffer = MediaFactory::createVideo(this);
  }

  // Re-initialize the framebuffer to current settings
  switch(myEventHandler->state())
  {
    case EventHandler::S_EMULATE:
    case EventHandler::S_MENU:
    case EventHandler::S_CMDMENU:
      myConsole->initializeVideo();
      if(showmessage)
      {
        switch(myFrameBuffer->type())
        {
          case kSoftBuffer:
            myFrameBuffer->showMessage("Software mode");
            break;
          case kGLBuffer:
            myFrameBuffer->showMessage("OpenGL mode");
            break;
        }
      }
      break;  // S_EMULATE, S_MENU, S_CMDMENU

    case EventHandler::S_LAUNCHER:
      myLauncher->initializeVideo();
      break;  // S_LAUNCHER

#ifdef DEBUGGER_SUPPORT
    case EventHandler::S_DEBUGGER:
      myDebugger->initializeVideo();
      break;  // S_DEBUGGER
#endif

    default:
      break;
  }

  // Setup the SDL joysticks (must be done after FrameBuffer is created)
  if(changeBuffer) myEventHandler->setupJoysticks();

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::toggleFrameBuffer()
{
#ifdef DISPLAY_OPENGL
  // First figure out which mode to switch to
  string video = mySettings->getString("video");
  if(video == "soft")
    video = "gl";
  else if(video == "gl")
    video = "soft";
  else   // a driver that doesn't exist was requested, so use software mode
    video = "soft";

  myEventHandler->handleEvent(Event::Pause, 0);

  // Remember the pause state
  bool pause = myEventHandler->isPaused();

  // Update the settings and create the framebuffer
  mySettings->setString("video", video);
  createFrameBuffer(true);  // show onscreen message

  // And re-pause the system
  myEventHandler->pause(pause);

  // The palette and phosphor info for the framebuffer will be lost
  // when a new framebuffer is created; we must restore it
  if(myConsole)
  {
    const Properties& props = myConsole->properties();
    bool enable = props.get(Display_Phosphor) == "YES";
    int blend = atoi(props.get(Display_PPBlend).c_str());
    myFrameBuffer->enablePhosphor(enable, blend);
    myConsole->setPalette(mySettings->getString("palette"));
  }
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::createSound()
{
  // Delete the old sound device
  delete mySound;  mySound = NULL;

  // And recreate a new sound device
  mySound = MediaFactory::createAudio(this);
#ifndef SOUND_SUPPORT
  mySettings->setBool("sound", false);
#endif

  // Re-initialize the sound object to current settings
  switch(myEventHandler->state())
  {
    case EventHandler::S_EMULATE:
    case EventHandler::S_MENU:
    case EventHandler::S_CMDMENU:
    case EventHandler::S_DEBUGGER:
      myConsole->initializeAudio();
      break;  // S_EMULATE, S_MENU, S_DEBUGGER

    default:
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool OSystem::createConsole(const string& romfile)
{
  // Delete any lingering console object
  delete myConsole;  myConsole = NULL;

  bool retval = false, showmessage = false;

  // If a blank ROM has been given, we reload the current one (assuming one exists)
  if(romfile == "")
  {
    showmessage = true;  // we show a message if a ROM is being reloaded
    if(myRomFile == "")
    {
      cerr << "ERROR: Rom file not specified ..." << endl;
      return false;
    }
  }
  else
    myRomFile = romfile;

  // Open the cartridge image and read it in
  uInt8* image;
  int size = -1;
  string md5;
  if(openROM(myRomFile, md5, &image, &size))
  {
    // Create an instance of the 2600 game console
    // The Console c'tor takes care of updating the eventhandler state
    myConsole = new Console(image, size, md5, this);
    if(myConsole && myConsole->isValid())
    {
    #ifdef CHEATCODE_SUPPORT
      myCheatManager->loadCheats(md5);
    #endif
      setFramerate(60);  // We need to set framerate to see messages
      myEventHandler->reset(EventHandler::S_EMULATE);
      createFrameBuffer(false);
      myFrameBuffer->setCursorState();
      myConsole->initialize();  // Must be done *after* the framebuffer is created

      if(showmessage)
        myFrameBuffer->showMessage("New console created");
      if(mySettings->getBool("showinfo"))
        cout << "Game console created: " << myRomFile << endl;

      retval = true;
    }
    else
    {
      cerr << "ERROR: Couldn't create console for " << myRomFile << " ..." << endl;
      retval = false;
    }
  }
  else
  {
    cerr << "ERROR: Couldn't open " << myRomFile << " ..." << endl;
    retval = false;
  }

  // Free the image since we don't need it any longer
  if(size != -1)
    delete[] image;

  return retval;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::createLauncher()
{
  mySound->close();
  setFramerate(60);
  myEventHandler->reset(EventHandler::S_LAUNCHER);
  createFrameBuffer(false);

  // And start the base dialog
  myLauncher->initialize();
  myLauncher->reStack();

  myEventHandler->refreshDisplay();

  myFrameBuffer->setCursorState();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool OSystem::openROM(const string& rom, string& md5, uInt8** image, int* size)
{
  // Try to open the file as a zipped archive
  // If that fails, we assume it's just a normal data file
  unzFile tz;
  if((tz = unzOpen(rom.c_str())) != NULL)
  {
    if(unzGoToFirstFile(tz) == UNZ_OK)
    {
      unz_file_info ufo;

      for(;;)  // Loop through all files for valid 2600 images
      {
        // Longer filenames might be possible, but I don't
        // think people would name files that long in zip files...
        char filename[1024];

        unzGetCurrentFileInfo(tz, &ufo, filename, 1024, 0, 0, 0, 0);
        filename[1023] = '\0';

        if(strlen(filename) >= 4)
        {
          // Grab 3-character extension
          char* ext = filename + strlen(filename) - 4;

          if(!BSPF_strcasecmp(ext, ".bin") || !BSPF_strcasecmp(ext, ".a26"))
            break;
        }

        // Scan the next file in the zip
        if(unzGoToNextFile(tz) != UNZ_OK)
          break;
      }

      // Now see if we got a valid image
      if(ufo.uncompressed_size <= 0)
      {
        unzClose(tz);
        return false;
      }
      *size  = ufo.uncompressed_size;
      *image = new uInt8[*size];

      // We don't have to check for any return errors from these functions,
      // since if there are, 'image' will not contain a valid ROM and the
      // calling method can take of it
      unzOpenCurrentFile(tz);
      unzReadCurrentFile(tz, *image, *size);
      unzCloseCurrentFile(tz);
      unzClose(tz);
    }
    else
    {
      unzClose(tz);
      return false;
    }
  }
  else
  {
    ifstream in(rom.c_str(), ios_base::binary);
    if(!in)
      return false;

    *image = new uInt8[512 * 1024];
    in.read((char*)(*image), 512 * 1024);
    *size = in.gcount();
    in.close();
  }

  // If we get to this point, we know we have a valid file to open
  // Now we make sure that the file has a valid properties entry
  md5 = MD5(*image, *size);

  // Some games may not have a name, since there may not
  // be an entry in stella.pro.  In that case, we use the rom name
  // and reinsert the properties object
  Properties props;
  myPropSet->getMD5(md5, props);

  string name = props.get(Cartridge_Name);
  if(name == "Untitled")
  {
    // Get the filename from the rom pathname
    string::size_type pos = rom.find_last_of(BSPF_PATH_SEPARATOR);
    if(pos+1 != string::npos)
    {
      name = rom.substr(pos+1);
      props.set(Cartridge_MD5, md5);
      props.set(Cartridge_Name, name);
      myPropSet->insert(props, false);
    }
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setDefaultJoymap()
{
  EventMode mode;

  mode = kEmulationMode;  // Default emulation events
  // Left joystick (assume joystick zero, button zero)
  myEventHandler->setDefaultJoyMapping(Event::JoystickZeroFire, mode, 0, 0);
  // Right joystick (assume joystick one, button zero)
  myEventHandler->setDefaultJoyMapping(Event::JoystickOneFire, mode, 1, 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setDefaultJoyAxisMap()
{
  EventMode mode;

  mode = kEmulationMode;  // Default emulation events
  // Left joystick left/right directions (assume joystick zero)
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickZeroLeft, mode, 0, 0, 0);
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickZeroRight, mode, 0, 0, 1);
  // Left joystick up/down directions (assume joystick zero)
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickZeroUp, mode, 0, 1, 0);
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickZeroDown, mode, 0, 1, 1);
  // Right joystick left/right directions (assume joystick one)
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickOneLeft, mode, 1, 0, 0);
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickOneRight, mode, 1, 0, 1);
  // Right joystick left/right directions (assume joystick one)
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickOneUp, mode, 1, 1, 0);
  myEventHandler->setDefaultJoyAxisMapping(Event::JoystickOneDown, mode, 1, 1, 1);

  mode = kMenuMode;  // Default menu/UI events
  myEventHandler->setDefaultJoyAxisMapping(Event::UILeft, mode, 0, 0, 0);
  myEventHandler->setDefaultJoyAxisMapping(Event::UIRight, mode, 0, 0, 1);
  myEventHandler->setDefaultJoyAxisMapping(Event::UIUp, mode, 0, 1, 0);
  myEventHandler->setDefaultJoyAxisMapping(Event::UIDown, mode, 0, 1, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setDefaultJoyHatMap()
{
// FIXME - add emul and UI events
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::pollEvent()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool OSystem::joyButtonHandled(int button)
{
  // Since we don't do any platform-specific event polling,
  // no button is ever handled at this level
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::stateChanged(EventHandler::State state)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::pauseChanged(bool status)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystem::OSystem(const OSystem& osystem)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystem& OSystem::operator = (const OSystem&)
{
  assert(false);

  return *this;
}
