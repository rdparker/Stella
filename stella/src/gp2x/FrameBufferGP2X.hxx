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
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: FrameBufferGP2X.hxx,v 1.5 2006-12-05 21:55:15 stephena Exp $
//============================================================================

#ifndef FRAMEBUFFER_GP2X_HXX
#define FRAMEBUFFER_GP2X_HXX

#include <SDL.h>

class OSystem;
class GUI::Font;

#include "bspf.hxx"
#include "FrameBuffer.hxx"


/**
  This class implements an SDL hardware framebuffer for the GP2X device.

  @author  Stephen Anthony
  @version $Id: FrameBufferGP2X.hxx,v 1.5 2006-12-05 21:55:15 stephena Exp $
*/
class FrameBufferGP2X : public FrameBuffer
{
  public:
    /**
      Creates a new software framebuffer
    */
    FrameBufferGP2X(OSystem* osystem);

    /**
      Destructor
    */
    virtual ~FrameBufferGP2X();

    //////////////////////////////////////////////////////////////////////
    // The following methods are derived from FrameBuffer.hxx
    //////////////////////////////////////////////////////////////////////
    /**
      This method is called to initialize software video mode.
      Return false if any operation fails, otherwise return true.
    */
    virtual bool initSubsystem();

    /**
      This method is called to query the type of the FrameBuffer.
    */
    virtual BufferType type() { return kSoftBuffer; }

    /**
      This method is called to set the aspect ratio of the screen.
    */
    virtual void setAspectRatio();

    /**
      This method is called to change to the given scaler type.

      @param scaler  The scaler to use for rendering the mediasource
    */
    virtual void setScaler(Scaler scaler);

    /**
      This method is called whenever the screen needs to be recreated.
      It updates the global screen variable.
    */
    virtual bool createScreen();

    /**
      Switches between the filtering options in software mode.
      Currently, none exist.
    */
    virtual void toggleFilter();

    /**
      This method should be called anytime the MediaSource needs to be redrawn
      to the screen.
    */
    virtual void drawMediaSource();

    /**
      This method is called before any drawing is done (per-frame).
    */
    virtual void preFrameUpdate();

    /**
      This method is called after any drawing is done (per-frame).
    */
    virtual void postFrameUpdate();

    /**
      This method is called to get the specified scanline data.

      @param row  The row we are looking for
      @param data The actual pixel data (in bytes)
    */
    virtual void scanline(uInt32 row, uInt8* data);

    /**
      This method is called to map a given r,g,b triple to the screen palette.

      @param r  The red component of the color.
      @param g  The green component of the color.
      @param b  The blue component of the color.
    */
    virtual Uint32 mapRGB(Uint8 r, Uint8 g, Uint8 b)
      { return SDL_MapRGB(myScreen->format, r, g, b); }

    /**
      This method is called to draw a horizontal line.

      @param x     The first x coordinate
      @param y     The y coordinate
      @param x2    The second x coordinate
      @param color The color of the line
    */
    virtual void hLine(uInt32 x, uInt32 y, uInt32 x2, int color);

    /**
      This method is called to draw a vertical line.

      @param x     The x coordinate
      @param y     The first y coordinate
      @param y2    The second y coordinate
      @param color The color of the line
    */
    virtual void vLine(uInt32 x, uInt32 y, uInt32 y2, int color);

    /**
      This method is called to draw a filled rectangle.

      @param x      The x coordinate
      @param y      The y coordinate
      @param w      The width of the area
      @param h      The height of the area
      @param color  The color of the area
    */
    virtual void fillRect(uInt32 x, uInt32 y, uInt32 w, uInt32 h,
                          int color);

    /**
      This method is called to draw the specified character.

      @param font   The font to use to draw the character
      @param c      The character to draw
      @param x      The x coordinate
      @param y      The y coordinate
      @param color  The color of the character
    */
    virtual void drawChar(const GUI::Font* font, uInt8 c, uInt32 x, uInt32 y,
                          int color);

    /**
      This method is called to draw the bitmap image.

      @param bitmap The data to draw
      @param x      The x coordinate
      @param y      The y coordinate
      @param color  The color of the character
      @param h      The height of the data image
    */
    virtual void drawBitmap(uInt32* bitmap, Int32 x, Int32 y, int color,
                            Int32 h = 8);

    /**
      This method translates the given coordinates to their
      unzoomed/unscaled equivalents.

      @param x  X coordinate to translate
      @param y  Y coordinate to translate
    */
    virtual void translateCoords(Int32* x, Int32* y);

    /**
      This method adds a dirty rectangle
      (ie, an area of the screen that has changed)

      @param x      The x coordinate
      @param y      The y coordinate
      @param w      The width of the area
      @param h      The height of the area
    */
    virtual void addDirtyRect(uInt32 x, uInt32 y, uInt32 w, uInt32 h);

    /**
      Enable/disable phosphor effect.
    */
    virtual void enablePhosphor(bool enable, int blend);

    /**
      Completely erase contents of the screen.
    */
    virtual void cls();

    /**
      Shows or hides the cursor based on the given boolean value.
    */
    virtual void showCursor(bool show);

  private:
    // Indicates that the buffer is dirty, and should be redrawn/flipped
    bool myDirtyFlag;

    // Pitch (in bytes) of the current screen
    int myPitch;
};

#endif
