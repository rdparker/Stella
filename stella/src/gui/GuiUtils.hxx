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
// $Id: GuiUtils.hxx,v 1.2 2005-03-14 04:08:15 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#ifndef UTILITIES_HXX
#define UTILITIES_HXX

#include "bspf.hxx"

/**
  A place to put GUI-related things that don't fit anywhere else.
  Probably not very neat, but at least it works ...

  @author  Stephen Anthony
  @version $Id: GuiUtils.hxx,v 1.2 2005-03-14 04:08:15 stephena Exp $
*/

// Colors indices to use for the various GUI elements
enum OverlayColor {
  kColor,
  kBGColor,
  kShadowColor,
  kTextColor,
  kTextColorHi
};

// Standard entries for OK/Cancel buttons
enum {
  kOKCmd    = 'ok  ',
  kCloseCmd = 'clos'
};

static const string EmptyString("");

#define kLineHeight 12

#endif
