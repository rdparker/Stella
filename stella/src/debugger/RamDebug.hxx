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
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#ifndef RAM_DEBUG_HXX
#define RAM_DEBUG_HXX

class System;

#include "Array.hxx"
#include "DebuggerSystem.hxx"

class RamState : public DebuggerState
{
  public:
    IntArray ram;
};

class RamDebug : public DebuggerSystem
{
  public:
    RamDebug(Debugger& dbg, Console& console);

    const DebuggerState& getState();
    const DebuggerState& getOldState() { return myOldState; }

    void saveOldState();
    string toString();

    int read(int offset);
    void write(int offset, int value);

  private:
    RamState myState;
    RamState myOldState;
};

#endif
