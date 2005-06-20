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
// $Id: DebuggerParser.hxx,v 1.12 2005-06-20 21:01:37 stephena Exp $
//============================================================================

#ifndef DEBUGGER_PARSER_HXX
#define DEBUGGER_PARSER_HXX

class Debugger;

#include "bspf.hxx"
#include "EquateList.hxx"

#define kMAX_ARGS 100

typedef enum {
	kBASE_16,
	kBASE_10,
	kBASE_2,
	kBASE_DEFAULT
} BaseFormat;

class DebuggerParser
{
	public:
		DebuggerParser(Debugger* debugger);
		~DebuggerParser();

		string run(const string& command);
		int decipher_arg(const string &str);

		void setBase(BaseFormat base) { defaultBase = base; }
		BaseFormat base()             { return defaultBase; }

	private:
		bool getArgs(const string& command);
		int conv_hex_digit(char d);
		bool subStringMatch(const string& needle, const string& haystack);
		string disasm();
		string listBreaks();
		string eval();

		Debugger* debugger;

		bool done;

		string verb;
		int args[kMAX_ARGS+1]; // FIXME: should be dynamic
		string argStrings[kMAX_ARGS+1];
		int argCount;
		BaseFormat defaultBase;
};

#endif
