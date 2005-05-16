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
// Copyright (c) 1995-1998 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: FrameBuffer.cxx,v 1.37 2005-05-16 00:02:31 stephena Exp $
//============================================================================

#include <sstream>

#include "bspf.hxx"
#include "Console.hxx"
#include "Event.hxx"
#include "EventHandler.hxx"
#include "StellaEvent.hxx"
#include "Settings.hxx"
#include "MediaSrc.hxx"
#include "FrameBuffer.hxx"
#include "StellaFont.hxx"
#include "GuiUtils.hxx"
#include "Menu.hxx"
#include "Launcher.hxx"
#include "OSystem.hxx"

#include "stella.xpm"   // The Stella icon

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FrameBuffer::FrameBuffer(OSystem* osystem)
   :  myOSystem(osystem),
      theRedrawEntireFrameIndicator(true),
      theZoomLevel(2),
      theMaxZoomLevel(2),
      theAspectRatio(1.0),
      theUseAspectRatioFlag(true),
      myFrameRate(0),
      myPauseStatus(false),
      theMenuChangedIndicator(false),
      myMessageTime(0),
      myMessageText(""),
      myMenuRedraws(2),
      myNumRedraws(0)
{
  // Fill the GUI colors array
  // The specific video subsystem will know what to do with it
  uInt8 colors[kNumColors][3] = {
    {104, 104, 104},
    {0, 0, 0},
    {64, 64, 64},
    {32, 160, 32},
    {0, 255, 0},
    {200, 0, 0}
  };

  for(uInt8 i = 0; i < kNumColors; i++)
    for(uInt8 j = 0; j < 3; j++)
      myGUIColors[i][j] = colors[i][j];

  // Create a font to draw text
  myFont = new StellaFont(this);

  myBaseDim.x = myBaseDim.y = myBaseDim.w = myBaseDim.h = 0;
  myImageDim = myScreenDim = myDesktopDim = myBaseDim;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FrameBuffer::~FrameBuffer(void)
{
  if(myFont)
    delete myFont;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::initialize(const string& title, uInt32 width, uInt32 height,
                             bool useAspect)
{
  bool isAlreadyInitialized = (SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) > 0;

  myBaseDim.w = (uInt16) width;
  myBaseDim.h = (uInt16) height;
  myFrameRate = myOSystem->frameRate();

  // Now (re)initialize the SDL video system
  if(!isAlreadyInitialized)
  {
    Uint32 initflags = SDL_INIT_VIDEO | SDL_INIT_TIMER;

    if(SDL_Init(initflags) < 0)
      return;

    setWindowIcon();
  }

  // Calculate the desktop size
  myDesktopDim.w = myDesktopDim.h = 0;

  // Get the system-specific WM information
  SDL_SysWMinfo myWMInfo;
  SDL_VERSION(&myWMInfo.version);
  if(SDL_GetWMInfo(&myWMInfo) > 0)
  {
#if defined(UNIX)
	if(myWMInfo.subsystem == SDL_SYSWM_X11)
	{
	  myWMInfo.info.x11.lock_func();
	  myDesktopDim.w = DisplayWidth(myWMInfo.info.x11.display,
						 DefaultScreen(myWMInfo.info.x11.display));
	  myDesktopDim.h = DisplayHeight(myWMInfo.info.x11.display,
						 DefaultScreen(myWMInfo.info.x11.display));
	  myWMInfo.info.x11.unlock_func();
	}
#elif defined(WIN32)
	myDesktopDim.w = (uInt16) GetSystemMetrics(SM_CXSCREEN);
	myDesktopDim.h = (uInt16) GetSystemMetrics(SM_CYSCREEN);
#elif defined(MAC_OSX)
	// FIXME - add OSX Desktop code here (I don't think SDL supports it yet)
#endif
  }

  // Set fullscreen flag
  mySDLFlags = myOSystem->settings().getBool("fullscreen") ? SDL_FULLSCREEN : 0;

  // Set window title
  setWindowTitle(title);

  // Indicate whether we want to use aspect ratio correction
  theUseAspectRatioFlag = useAspect;

  // Get the maximum size of a window for the current desktop
  theMaxZoomLevel = maxWindowSizeForScreen();

  // Check to see if window size will fit in the screen
  if((uInt32)myOSystem->settings().getInt("zoom") > theMaxZoomLevel)
    theZoomLevel = theMaxZoomLevel;
  else
    theZoomLevel = myOSystem->settings().getInt("zoom");

  // Initialize video subsystem
  initSubsystem();

  // Erase any messages from a previous run
  myMessageTime = 0;
  theRedrawEntireFrameIndicator = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::update()
{
  // Do any pre-frame stuff
  preFrameUpdate();

  // Determine which mode we are in (from the EventHandler)
  // Take care of S_EMULATE mode here, otherwise let the GUI
  // figure out what to draw
  switch(myOSystem->eventHandler().state())
  {
    case EventHandler::S_EMULATE:
    {
      // Draw changes to the mediasource
      if(!myPauseStatus)
        myOSystem->console().mediaSource().update();

      // We always draw the screen, even if the core is paused
      drawMediaSource();

      if(!myPauseStatus)
      {
        // Draw any pending messages
        if(myMessageTime > 0)
        {
          int w = myFont->getStringWidth(myMessageText) + 10;
          int h = myFont->getFontHeight() + 8;
          int x = (myBaseDim.w >> 1) - (w >> 1);
          int y = myBaseDim.h - h - 10/2;

          // Draw the bounded box and text
          blendRect(x+1, y+2, w-2, h-4, kBGColor);
          box(x, y+1, w, h-2, kColor, kColor);
          myFont->drawString(myMessageText, x+1, y+4, w, kTextColor, kTextAlignCenter);
          myMessageTime--;

          // Erase this message on next update
          if(myMessageTime == 0)
            theRedrawEntireFrameIndicator = true;
        }
      }
      break;  // S_EMULATE
    }

    case EventHandler::S_MENU:
    {
      // Only update the screen if it's been invalidated or the menus have changed  
      if(theRedrawEntireFrameIndicator || theMenuChangedIndicator)
      {
        drawMediaSource();

        // Then overlay any menu items
        myOSystem->menu().draw();

        // Now the screen is up to date
        theRedrawEntireFrameIndicator = false;

        // This is a performance hack to only draw the menus when necessary
        // Software mode is single-buffered, so we don't have to worry
        // However, OpenGL mode is double-buffered, so we need to draw the
        // menus at least twice (so they'll be in both buffers)
        // Otherwise, we get horrible flickering
        myMenuRedraws--;
        theMenuChangedIndicator = (myMenuRedraws != 0);
      }
      break;
    }

    case EventHandler::S_LAUNCHER:
    {
      // Only update the screen if it's been invalidated or the menus have changed  
      if(theRedrawEntireFrameIndicator || theMenuChangedIndicator)
      {
        // Overlay the ROM launcher
        myOSystem->launcher().draw();

        // Now the screen is up to date
        theRedrawEntireFrameIndicator = false;

        // This is a performance hack to only draw the menus when necessary
        // Software mode is single-buffered, so we don't have to worry
        // However, OpenGL mode is double-buffered, so we need to draw the
        // menus at least twice (so they'll be in both buffers)
        // Otherwise, we get horrible flickering
        myMenuRedraws--;
        theMenuChangedIndicator = (myMenuRedraws != 0);
      }
      break;
    }

    case EventHandler::S_DEBUGGER:
      // Not yet implemented
      break;

    case EventHandler::S_NONE:
      return;
      break;
  }

  // Do any post-frame stuff
  postFrameUpdate();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::showMessage(const string& message)
{
  myMessageText = message;
  myMessageTime = myFrameRate << 1;   // Show message for 2 seconds
  theRedrawEntireFrameIndicator = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::pause(bool status)
{
  myPauseStatus = status;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::setPalette(const uInt32* palette)
{
  for(uInt32 i = 0; i < 256; ++i)
  {
    Uint8 r, g, b;

    r = (Uint8) ((palette[i] & 0x00ff0000) >> 16);
    g = (Uint8) ((palette[i] & 0x0000ff00) >> 8);
    b = (Uint8) (palette[i] & 0x000000ff);

    myPalette[i] = mapRGB(r, g, b);
  }

  theRedrawEntireFrameIndicator = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::toggleFullscreen()
{
  setFullscreen(!myOSystem->settings().getBool("fullscreen"));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::setFullscreen(bool enable)
{
  // Update the settings
  myOSystem->settings().setBool("fullscreen", enable);

  if(enable)
    mySDLFlags |= SDL_FULLSCREEN;
  else
    mySDLFlags &= ~SDL_FULLSCREEN;

  if(!createScreen())
    return;

  setCursorState();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::resize(Size size, Int8 zoom)
{
  switch(size)
  {
    case PreviousSize:   // decrease size
      if(myOSystem->settings().getBool("fullscreen"))
        return;
      if(theZoomLevel == 1)
        theZoomLevel = theMaxZoomLevel;
      else
        theZoomLevel--;
      break;

    case NextSize:       // increase size
      if(myOSystem->settings().getBool("fullscreen"))
        return;
      if(theZoomLevel == theMaxZoomLevel)
        theZoomLevel = 1;
      else
        theZoomLevel++;
      break;

    case GivenSize:      // use 'zoom' quantity
      if(zoom < 1)
        theZoomLevel = 1;
      else if((uInt32)zoom > theMaxZoomLevel)
        theZoomLevel = theMaxZoomLevel;
      else
        theZoomLevel = zoom;
      break;

    default:   // should never happen
      return;
      break;
  }

  if(!createScreen())
    return;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::setCursorState()
{
  bool isFullscreen = myOSystem->settings().getBool("fullscreen");

  if(isFullscreen)
    grabMouse(true);
  else
    grabMouse(myOSystem->settings().getBool("grabmouse"));

  switch(myOSystem->eventHandler().state())
  {
    case EventHandler::S_EMULATE:
      showCursor(false);
      break;

    default:
      showCursor(true);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::showCursor(bool show)
{
  if(show)
    SDL_ShowCursor(SDL_ENABLE);
  else
    SDL_ShowCursor(SDL_DISABLE);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::grabMouse(bool grab)
{
  if(grab)
    SDL_WM_GrabInput(SDL_GRAB_ON);
  else
    SDL_WM_GrabInput(SDL_GRAB_OFF);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FrameBuffer::fullScreen()
{
  return myOSystem->settings().getBool("fullscreen");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 FrameBuffer::maxWindowSizeForScreen()
{
  uInt32 sWidth     = myDesktopDim.w;
  uInt32 sHeight    = myDesktopDim.h;
  uInt32 multiplier = sWidth / myBaseDim.w;

  // If screenwidth or height could not be found, use default zoom value
  if(sWidth == 0 || sHeight == 0)
    return 4;

  bool found = false;
  while(!found && (multiplier > 0))
  {
    // Figure out the desired size of the window
    uInt32 width  = myBaseDim.w * multiplier;
    uInt32 height = myBaseDim.h * multiplier;

    if((width < sWidth) && (height < sHeight))
      found = true;
    else
      multiplier--;
  }

  if(found)
    return multiplier;
  else
    return 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::setWindowTitle(const string& title)
{
  SDL_WM_SetCaption(title.c_str(), "stella");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::setWindowIcon()
{
#ifndef MAC_OSX
  // Set the window icon
  uInt32 w, h, ncols, nbytes;
  uInt32 rgba[256], icon[32 * 32];
  uInt8  mask[32][4];

  sscanf(stella_icon[0], "%d %d %d %d", &w, &h, &ncols, &nbytes);
  if((w != 32) || (h != 32) || (ncols > 255) || (nbytes > 1))
  {
    cerr << "ERROR: Couldn't load the icon.\n";
    return;
  }

  for(uInt32 i = 0; i < ncols; i++)
  {
    unsigned char code;
    char color[32];
    uInt32 col;

    sscanf(stella_icon[1 + i], "%c c %s", &code, color);
    if(!strcmp(color, "None"))
      col = 0x00000000;
    else if(!strcmp(color, "black"))
      col = 0xFF000000;
    else if (color[0] == '#')
    {
      sscanf(color + 1, "%06x", &col);
      col |= 0xFF000000;
    }
    else
    {
      cerr << "ERROR: Couldn't load the icon.\n";
      return;
    }
    rgba[code] = col;
  }

  memset(mask, 0, sizeof(mask));
  for(h = 0; h < 32; h++)
  {
    const char* line = stella_icon[1 + ncols + h];
    for(w = 0; w < 32; w++)
    {
      icon[w + 32 * h] = rgba[(int)line[w]];
      if(rgba[(int)line[w]] & 0xFF000000)
        mask[h][w >> 3] |= 1 << (7 - (w & 0x07));
    }
  }

  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(icon, 32, 32, 32,
                         32 * 4, 0xFF0000, 0x00FF00, 0x0000FF, 0xFF000000);
  SDL_WM_SetIcon(surface, (unsigned char *) mask);
  SDL_FreeSurface(surface);
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::box(uInt32 x, uInt32 y, uInt32 w, uInt32 h,
                      OverlayColor colorA, OverlayColor colorB)
{
  hLine(x + 1, y,     x + w - 2, colorA);
  hLine(x,     y + 1, x + w - 1, colorA);
  vLine(x,     y + 1, y + h - 2, colorA);
  vLine(x + 1, y,     y + h - 1, colorA);

  hLine(x + 1,     y + h - 2, x + w - 1, colorB);
  hLine(x + 1,     y + h - 1, x + w - 2, colorB);
  vLine(x + w - 1, y + 1,     y + h - 2, colorB);
  vLine(x + w - 2, y + 1,     y + h - 1, colorB);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBuffer::frameRect(uInt32 x, uInt32 y, uInt32 w, uInt32 h,
                            OverlayColor color)
{
  hLine(x,         y,         x + w - 1, color);
  hLine(x,         y + h - 1, x + w - 1, color);
  vLine(x,         y,         y + h - 1, color);
  vLine(x + w - 1, y,         y + h - 1, color);
}
