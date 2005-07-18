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
// $Id: TIADebug.hxx,v 1.10 2005-07-18 16:10:52 urchlay Exp $
//============================================================================

#ifndef TIA_DEBUG_HXX
#define TIA_DEBUG_HXX

class TIA;
class Debugger;

#include "Array.hxx"
#include "DebuggerSystem.hxx"

/*
// pointer types for TIADebug instance methods
// (used by IntMethodExpression)
typedef int (TiaDebug::*TIADEBUG_INT_METHOD)();

// call the pointed-to method on the (global) CPU debugger object.
#define CALL_TIADEBUG_METHOD(method) ( ( Debugger::debugger().tiaDebug().*method)() )
*/

class TiaState : public DebuggerState
{
  public:
    IntArray ram;
    IntArray coluRegs;
};

class TIADebug : public DebuggerSystem
{
  public:
    TIADebug(Debugger* dbg, Console* console);

    DebuggerState& getState();
    DebuggerState& getOldState() { return myOldState; }

    void saveOldState();

    void clearTIA();

    // FIXME - add whole slew of setXXX() methods
    uInt8 coluP0(int newVal = -1);
    uInt8 coluP1(int newVal = -1);
    uInt8 coluPF(int newVal = -1);
    uInt8 coluBK(int newVal = -1);

    int scanlines();
    int frameCount();
    bool vsync();
    bool vblank();
    void updateTIA();
    string state();

  private:
    string colorSwatch(uInt8 c);

  private:
    TiaState myState;
    TiaState myOldState;

    System* mySystem;
    TIA* myTIA;

    string nusizStrings[8];
};

#endif
