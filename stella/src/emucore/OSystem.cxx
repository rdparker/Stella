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
// Copyright (c) 1995-2005 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: OSystem.cxx,v 1.8 2005-05-01 18:57:20 stephena Exp $
//============================================================================

#include <cassert>
#include <sstream>
#include <fstream>

#include "FrameBuffer.hxx"
#include "FrameBufferSoft.hxx"
#ifdef DISPLAY_OPENGL
  #include "FrameBufferGL.hxx"
#endif

#include "Sound.hxx"
#include "SoundNull.hxx"
#ifdef SOUND_SUPPORT
  #include "SoundSDL.hxx"
#endif

#include "Settings.hxx"
#include "PropsSet.hxx"
#include "EventHandler.hxx"
#include "Menu.hxx"
//#include "Browser.hxx"
#include "bspf.hxx"
#include "OSystem.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystem::OSystem()
  : myEventHandler(NULL),
    myFrameBuffer(NULL),
    mySound(NULL),
    mySettings(NULL),
    myPropSet(NULL),
    myConsole(NULL),
    myMenu(NULL)
//    myBrowser(NULL)
{
  // Create gui-related classes
  myMenu = new Menu(this);
//  myBrowser = new Browser(this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OSystem::~OSystem()
{
  delete myMenu;
//  delete myBrowser;

  // Remove any game console that is currently attached
  detachConsole();

  // OSystem takes responsibility for framebuffer and sound,
  // since it created them
  delete myFrameBuffer;
  delete mySound;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setPropertiesFiles(const string& userprops,
                                 const string& systemprops)
{
  // Set up the input and output properties files
  myPropertiesOutputFile = userprops;

  if(fileExists(userprops))
    myPropertiesInputFile = userprops;
  else if(fileExists(systemprops))
    myPropertiesInputFile = systemprops;
  else
    myPropertiesInputFile = "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::setConfigFiles(const string& userconfig,
                             const string& systemconfig)
{
  // Set up the names of the input and output config files
  myConfigOutputFile = userconfig;

  if(fileExists(userconfig))
    myConfigInputFile = userconfig;
  else if(fileExists(systemconfig))
    myConfigInputFile = systemconfig;
  else
    myConfigInputFile = "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool OSystem::createFrameBuffer(bool showmessage)
{
  // Set the SDL_VIDEODRIVER environment variable, if possible
  string videodriver = mySettings->getString("video_driver");
  if(videodriver != "")
  {
    string buf = "SDL_VIDEODRIVER=" + videodriver;
    putenv((char*) buf.c_str());

    if(mySettings->getBool("showinfo"))
    {
      buf = "Video driver: " + videodriver;
      cout << buf << endl;
    }
  }

  // Delete the old framebuffer
  if(myFrameBuffer)
  {
    delete myFrameBuffer;
    myFrameBuffer = NULL;
  }

  // And recreate a new one
  string video = mySettings->getString("video");
  if(video == "soft")
    myFrameBuffer = new FrameBufferSoft(this);
#ifdef DISPLAY_OPENGL
  else if(video == "gl")
    myFrameBuffer = new FrameBufferGL(this);
#endif
  else   // a driver that doesn't exist was requested, so use software mode
    myFrameBuffer = new FrameBufferSoft(this);

  if(!myFrameBuffer)
    return false;

  // Re-initialize the framebuffer to current settings
  switch(myEventHandler->state())
  {
    case EventHandler::S_EMULATE:
    case EventHandler::S_MENU:
      myConsole->initializeVideo();
      if(showmessage)
      {
        if(video == "soft")
          myFrameBuffer->showMessage("Software mode");
      #ifdef DISPLAY_OPENGL
        else if(video == "gl")
          myFrameBuffer->showMessage("OpenGL mode");
      #endif
        else   // a driver that doesn't exist was requested, so use software mode
          myFrameBuffer->showMessage("Software mode");
      }
      break;  // S_EMULATE, S_MENU

    case EventHandler::S_BROWSER:
      break;  // S_BROWSER

    case EventHandler::S_DEBUGGER:
      break;

    case EventHandler::S_NONE:
      break;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::toggleFrameBuffer()
{
  // First figure out which mode to switch to
  string video = mySettings->getString("video");
  if(video == "soft")
    video = "gl";
  else if(video == "gl")
    video = "soft";
  else   // a driver that doesn't exist was requested, so use software mode
    video = "soft";

  // Update the settings and create the framebuffer
  mySettings->setString("video", video);
  createFrameBuffer(true);  // show onscreen message
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OSystem::createSound()
{
  // Delete the old sound device
  delete mySound;

  // And recreate a new sound device
#ifdef SOUND_SUPPORT
  mySound = new SoundSDL(this);
#else
  mySettings->setBool("sound", false);
  mySound = new SoundNull(this);
#endif

  // Re-initialize the framebuffer to current settings
  switch(myEventHandler->state())
  {
    case EventHandler::S_EMULATE:
    case EventHandler::S_MENU:
      myConsole->initializeAudio();
      break;  // S_EMULATE, S_MENU

    case EventHandler::S_BROWSER:
      break;  // S_BROWSER

    case EventHandler::S_DEBUGGER:
      break;

    case EventHandler::S_NONE:
      break;
  }
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
