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
// $Id: GuiObject.hxx,v 1.12 2005-06-16 00:55:59 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#ifndef GUI_OBJECT_HXX
#define GUI_OBJECT_HXX

class OSystem;
class DialogContainer;
class Widget;

#include "Command.hxx"

/**
  This is the base class for all GUI objects/widgets.
  
  @author  Stephen Anthony
  @version $Id: GuiObject.hxx,v 1.12 2005-06-16 00:55:59 stephena Exp $
*/
class GuiObject : public CommandReceiver
{
  friend class Widget;
  friend class DialogContainer;

  public:
    GuiObject(OSystem* osystem, DialogContainer* parent, int x, int y, int w, int h)
        : myOSystem(osystem),
          myParent(parent),
          _x(x),
          _y(y),
          _w(w),
          _h(h),
          _firstWidget(0) { }

    virtual ~GuiObject() { }

    OSystem* instance() { return myOSystem; }
    DialogContainer* parent() { return myParent; }

    virtual int getAbsX() const     { return _x; }
    virtual int getAbsY() const     { return _y; }
    virtual int getChildX() const   { return getAbsX(); }
    virtual int getChildY() const   { return getAbsY(); }
    virtual int getWidth() const    { return _w; }
    virtual int getHeight() const   { return _h; }

    virtual void setWidth(int w)    { _w = w; }
    virtual void setHeight(int h)   { _h = h; }

    virtual bool isVisible() const = 0;
    virtual void draw() = 0;

    static void resetActiveWidget() { _activeWidget = NULL; }

  protected:
    OSystem*         myOSystem;
    DialogContainer* myParent;

    int _x, _y;
    int _w, _h;

    Widget* _firstWidget;
    static Widget* _activeWidget;

  protected:
    virtual void releaseFocus() = 0;
};

#endif
