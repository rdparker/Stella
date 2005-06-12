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
// $Id: DebuggerCommand.hxx,v 1.1 2005-06-12 18:18:00 stephena Exp $
//============================================================================

#ifndef DEBUGGER_COMMAND_HXX
#define DEBUGGER_COMMAND_HXX

#include "bspf.hxx"

//class DebuggerParser;
class Debugger;

class DebuggerCommand
{
	public:
		DebuggerCommand();
		DebuggerCommand(Debugger* d);

		virtual string getName() = 0;
		virtual int getArgCount() = 0;
		virtual string execute(int c, int *a) = 0;

	protected:
		int *args;
		Debugger *debugger;
//		DebuggerParser *parser;
};

#endif
