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
// $Id: DebuggerParser.hxx,v 1.13 2005-06-21 00:13:49 urchlay Exp $
//============================================================================

#ifndef DEBUGGER_PARSER_HXX
#define DEBUGGER_PARSER_HXX

class Debugger;

#include "bspf.hxx"
#include "EquateList.hxx"

#define kMAX_ARGS 100
#define kMAX_WATCHES 10

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
		string showWatches();
		string addWatch(string watch);
		string delWatch(int which);
		void delAllWatches();


	private:
		bool getArgs(const string& command);
		int conv_hex_digit(char d);
		bool subStringMatch(const string& needle, const string& haystack);
		string disasm();
		string listBreaks();
		string eval();
		string dump();

		Debugger* debugger;

		bool done;

		string verb;
		int args[kMAX_ARGS+1]; // FIXME: should be dynamic
		string argStrings[kMAX_ARGS+1];
		int argCount;
		BaseFormat defaultBase;
		string *watches[kMAX_WATCHES]; // FIXME: remove the limit sometime
};

#endif
