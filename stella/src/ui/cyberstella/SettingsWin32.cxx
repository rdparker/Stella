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
// Copyright (c) 1995-2002 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: SettingsWin32.cxx,v 1.2 2003-09-23 19:41:16 stephena Exp $
//============================================================================

#include "bspf.hxx"
#include "SettingsWin32.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SettingsWin32::SettingsWin32()
{
  mySettingsInputFilename = "stellarc";
  mySettingsOutputFilename = "stellarc";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SettingsWin32::~SettingsWin32()
{
        cerr << "SettingsWin32::~SettingsWin32()\n";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string SettingsWin32::stateFilename(uInt32 state)
{
        cerr << "SettingsWin32::stateFilename()\n";

        return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string SettingsWin32::snapshotFilename()
{
        cerr << "SettingsWin32::snapshotFilename()\n";

        return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SettingsWin32::usage(string& message)
{
}
