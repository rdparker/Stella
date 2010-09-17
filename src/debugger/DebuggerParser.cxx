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
// Copyright (c) 1995-2010 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#include <fstream>
#include "bspf.hxx"

#include "Dialog.hxx"
#include "Debugger.hxx"
#include "CartDebug.hxx"
#include "CpuDebug.hxx"
#include "RiotDebug.hxx"
#include "TIADebug.hxx"
#include "DebuggerParser.hxx"
#include "YaccParser.hxx"
#include "M6502.hxx"
#include "Expression.hxx"
#include "FSNode.hxx"
#include "RomWidget.hxx"
#include "ProgressDialog.hxx"

#ifdef CHEATCODE_SUPPORT
  #include "CheatManager.hxx"
#endif

#include "DebuggerParser.hxx"

// Call the pointed-to method on the this object. Whew.
#define CALL_METHOD(method) ( (this->*method)() )


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DebuggerParser::DebuggerParser(Debugger* d)
  : debugger(d)
{
  defaultBase = kBASE_16;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DebuggerParser::~DebuggerParser()
{
  args.clear();
  argStrings.clear();
  watches.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// main entry point: PromptWidget calls this method.
string DebuggerParser::run(const string& command)
{
  /*
    // this was our parser test code. Left for reference.
  static Expression *lastExpression;

  // special case: parser testing
  if(strncmp(command.c_str(), "expr ", 5) == 0) {
    delete lastExpression;
    commandResult = "parser test: status==";
    int status = YaccParser::parse(command.c_str() + 5);
    commandResult += debugger->valueToString(status);
    commandResult += ", result==";
    if(status == 0) {
      lastExpression = YaccParser::getResult();
      commandResult += debugger->valueToString(lastExpression->evaluate());
    } else {
      //  delete lastExpression; // NO! lastExpression isn't valid (not 0 either)
                                // It's the result of casting the last token
                                // to Expression* (because of yacc's union).
                                // As such, we can't and don't need to delete it
                                // (However, it means yacc leaks memory on error)
      commandResult += "ERROR - ";
      commandResult += YaccParser::errorMessage();
    }
    return commandResult;
  }

  if(command == "expr") {
    if(lastExpression)
      commandResult = "result==" + debugger->valueToString(lastExpression->evaluate());
    else
      commandResult = "no valid expr";
    return commandResult;
  }
  */

  string verb;
  getArgs(command, verb);
#ifdef EXPR_REF_COUNT
  extern int refCount;
  cerr << "Expression count: " << refCount << endl;
#endif
  commandResult.str("");

  for(int i = 0; i < kNumCommands; ++i)
  {
    if(verb == commands[i].cmdString)
    {
      if(validateArgs(i))
        CALL_METHOD(commands[i].executor);

      if(commands[i].refreshRequired)
        debugger->myBaseDialog->loadConfig();

      return commandResult.str();
    }
  }

  return "No such command (try \"help\")";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string DebuggerParser::exec(const FilesystemNode& file)
{
  if(file.exists())
  {
    ifstream in(file.getPath().c_str());
    if(!in.is_open())
      return red("file \'" + file.getRelativePath() + "\' not found.");

    ostringstream buf;
    int count = 0;
    char buffer[256];
    while( !in.eof() )
    {
      if(!in.getline(buffer, 255))
        break;

      count++;
    }
    buf << "Executed " << debugger->valueToString(count) << " commands from \""
        << file.getRelativePath() << "\"";

    return buf.str();
  }
  else
    return red("file \'" + file.getRelativePath() + "\' not found.");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Completion-related stuff:
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DebuggerParser::getCompletions(const char* in, StringList& completions) const
{
  // cerr << "Attempting to complete \"" << in << "\"" << endl;
  for(int i = 0; i < kNumCommands; ++i)
  {
    const char* l = commands[i].cmdString.c_str();
    if(BSPF_strncasecmp(l, in, strlen(in)) == 0)
      completions.push_back(l);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Evaluate expression. Expressions always evaluate to a 16-bit value if
// they're valid, or -1 if they're not.
// decipher_arg may be called by the GUI as needed. It is also called
// internally by DebuggerParser::run()
int DebuggerParser::decipher_arg(const string &str)
{
  bool derefByte=false, derefWord=false, lobyte=false, hibyte=false, bin=false, dec=false;
  int result;
  string arg = str;

  if(defaultBase == kBASE_2) {
    bin=true; dec=false;
  } else if(defaultBase == kBASE_10) {
    bin=false; dec=true;
  } else {
    bin=false; dec=false;
  }

  if(arg.substr(0, 1) == "*") {
    derefByte = true;
    arg.erase(0, 1);
  } else if(arg.substr(0, 1) == "@") {
    derefWord = true;
    arg.erase(0, 1);
  }

  if(arg.substr(0, 1) == "<") {
    lobyte = true;
    arg.erase(0, 1);
  } else if(arg.substr(0, 1) == ">") {
    hibyte = true;
    arg.erase(0, 1);
  }

  if(arg.substr(0, 1) == "%") {
    bin = true;
    dec = false;
    arg.erase(0, 1);
  } else if(arg.substr(0, 1) == "#") {
    dec = true;
    bin = false;
    arg.erase(0, 1);
  } else if(arg.substr(0, 1) == "$") {
    dec = false;
    bin = false;
    arg.erase(0, 1);
  }

  // sanity check mutually exclusive options:
  if(derefByte && derefWord) return -1;
  if(lobyte && hibyte) return -1;
  if(bin && dec) return -1;

  // Special cases (registers):
  CpuState& state = (CpuState&) debugger->cpuDebug().getState();
  if(arg == "a") result = state.A;
  else if(arg == "x") result = state.X;
  else if(arg == "y") result = state.Y;
  else if(arg == "p") result = state.PS;
  else if(arg == "s") result = state.SP;
  else if(arg == "pc" || arg == ".") result = state.PC;
  else { // Not a special, must be a regular arg: check for label first
    const char* a = arg.c_str();
    result = debugger->cartDebug().getAddress(arg);

    if(result < 0) { // if not label, must be a number
      if(bin) { // treat as binary
        result = 0;
        while(*a != '\0') {
          result <<= 1;
          switch(*a++) {
            case '1':
              result++;
              break;

            case '0':
              break;

            default:
              return -1;
          }
        }
      } else if(dec) {
        result = 0;
        while(*a != '\0') {
          int digit = (*a++) - '0';
          if(digit < 0 || digit > 9)
            return -1;

          result = (result * 10) + digit;
        }
      } else { // must be hex.
        result = 0;
        while(*a != '\0') {
          int hex = Debugger::conv_hex_digit(*a++);
          if(hex < 0)
            return -1;

          result = (result << 4) + hex;
        }
      }
    }
  }

  if(lobyte) result &= 0xff;
  else if(hibyte) result = (result >> 8) & 0xff;

  // dereference if we're supposed to:
  if(derefByte) result = debugger->peek(result);
  if(derefWord) result = debugger->dpeek(result);

  return result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string DebuggerParser::showWatches()
{
  string ret;
  char buf[10];

  for(unsigned int i=0; i<watches.size(); i++) {
    if(watches[i] != "") {
      // Clear the args, since we're going to pass them to eval()
      argStrings.clear();
      args.clear();

      sprintf(buf, "%d", i+1);
      argCount = 1;
      argStrings.push_back(watches[i]);
      args.push_back(decipher_arg(argStrings[0]));
      if(args[0] < 0) {
        ret += "BAD WATCH ";
        ret += buf;
        ret += ": " + argStrings[0] + "\n";
      } else {
        ret += " watch #";
        ret += buf;
        ret += " (" + argStrings[0] + ") -> " + eval() + "\n";
      }
    }
  }
  return ret;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Private methods below
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool DebuggerParser::getArgs(const string& command, string& verb)
{
  int state = kIN_COMMAND, i = 0, length = command.length();
  string curArg = "";
  verb = "";

  argStrings.clear();
  args.clear();

  // cerr << "Parsing \"" << command << "\"" << ", length = " << command.length() << endl;

  // First, pick apart string into space-separated tokens.
  // The first token is the command verb, the rest go in an array
  do
  {
    char c = command[i++];
    switch(state)
    {
      case kIN_COMMAND:
        if(c == ' ')
          state = kIN_SPACE;
        else
          verb += c;
        break;
      case kIN_SPACE:
        if(c == '{')
          state = kIN_BRACE;
        else if(c != ' ') {
          state = kIN_ARG;
          curArg += c;
        }
        break;
      case kIN_BRACE:
        if(c == '}') {
          state = kIN_SPACE;
          argStrings.push_back(curArg);
          //  cerr << "{" << curArg << "}" << endl;
          curArg = "";
        } else {
          curArg += c;
        }
        break;
      case kIN_ARG:
        if(c == ' ') {
          state = kIN_SPACE;
          argStrings.push_back(curArg);
          curArg = "";
        } else {
          curArg += c;
        }
        break;
    }  // switch(state)
  }
  while(i < length);

  // Take care of the last argument, if there is one
  if(curArg != "")
    argStrings.push_back(curArg);

  argCount = argStrings.size();
  /*
  cerr << "verb = " << verb << endl;
  cerr << "arguments (" << argCount << "):\n";
  for(int x = 0; x < argCount; x++)
    cerr << "command " << x << ": " << argStrings[x] << endl;
  */

  /*
  // Now decipher each argument, in turn.
  for(int i=0; i<argCount; i++) {
    int temp = decipher_arg(argStrings[i]);
    args.push_back(temp); // value maybe -1, if not expression argument
                          // (validate_args will decide whether that's OK, not us.)
  }
  */

  for(int i = 0; i < argCount; i++) {
    int err = YaccParser::parse(argStrings[i].c_str());
    if(err) {
      args.push_back(-1);
    } else {
      Expression* e = YaccParser::getResult();
      args.push_back( e->evaluate() );
      delete e;
    }
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool DebuggerParser::validateArgs(int cmd)
{
  // cerr << "entering validateArgs(" << cmd << ")" << endl;
  bool required = commands[cmd].parmsRequired;
  parameters *p = commands[cmd].parms;

  if(argCount == 0)
  {
    if(required)
    {
      commandResult.str(red("missing required argument(s)"));
      return false; // needed args. didn't get 'em.
    }
    else
      return true;  // no args needed, no args got
  }

  // Figure out how many arguments are required by the command
  int count = 0, argRequiredCount = 0;
  while(*p != kARG_END_ARGS && *p != kARG_MULTI_BYTE)
  {
    count++;
    *p++;
  }

  // Evil hack: some commands intentionally take multiple arguments
  // In this case, the required number of arguments is unbounded
  argRequiredCount = (*p == kARG_END_ARGS) ? count : argCount;

  p = commands[cmd].parms;
  int curCount = 0;

  do {
    if(curCount >= argCount)
      break;

    int curArgInt     = args[curCount];
    string& curArgStr = argStrings[curCount];

    switch(*p)
    {
      case kARG_WORD:
        if(curArgInt < 0 || curArgInt > 0xffff)
        {
          commandResult.str(red("invalid word argument (must be 0-$ffff)"));
          return false;
        }
        break;

      case kARG_BYTE:
        if(curArgInt < 0 || curArgInt > 0xff)
        {
          commandResult.str(red("invalid byte argument (must be 0-$ff)"));
          return false;
        }
        break;

      case kARG_BOOL:
        if(curArgInt != 0 && curArgInt != 1)
        {
          commandResult.str(red("invalid boolean argument (must be 0 or 1)"));
          return false;
        }
        break;

      case kARG_BASE_SPCL:
        if(curArgInt != 2 && curArgInt != 10 && curArgInt != 16
           && curArgStr != "hex" && curArgStr != "dec" && curArgStr != "bin")
        {
          commandResult.str(red("invalid base (must be #2, #10, #16, \"bin\", \"dec\", or \"hex\")"));
          return false;
        }
        break;

      case kARG_LABEL:
      case kARG_FILE:
        break; // TODO: validate these (for now any string's allowed)

      case kARG_MULTI_BYTE:
      case kARG_MULTI_WORD:
        break; // FIXME: validate these (for now, any number's allowed)

      case kARG_END_ARGS:
        break;
    }
    curCount++;
    *p++;

  } while(*p != kARG_END_ARGS && curCount < argRequiredCount);

/*
cerr << "curCount         = " << curCount << endl
     << "argRequiredCount = " << argRequiredCount << endl
     << "*p               = " << *p << endl << endl;
*/

  if(curCount < argRequiredCount)
  {
    commandResult.str(red("missing required argument(s)"));
    return false;
  }
  else if(argCount > curCount)
  {
    commandResult.str(red("too many arguments"));
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string DebuggerParser::eval()
{
  char buf[50];
  string ret;
  for(int i=0; i<argCount; i++) {
    // TODO - technically, we should determine if the label is read or write
    const string& label = debugger->cartDebug().getLabel(args[i], true);
    if(label != "") {
      ret += label;
      ret += ": ";
    }
    ret += "$";
    if(args[i] < 0x100) {
      ret += Debugger::to_hex_8(args[i]);
      ret += " %";
      ret += Debugger::to_bin_8(args[i]);
    } else {
      ret += Debugger::to_hex_16(args[i]);
      ret += " %";
      ret += Debugger::to_bin_16(args[i]);
    }
    sprintf(buf, " #%d", args[i]);
    ret += buf;
    if(i != argCount - 1) ret += "\n";
  }
  return ret;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string DebuggerParser::trapStatus(int addr)
{
  string result;
  result += debugger->valueToString(addr);
  result += ": ";
  bool r = debugger->readTrap(addr);
  bool w = debugger->writeTrap(addr);
  if(r && w)
    result += "read|write";
  else if(r)
    result += "read";
  else if(w)
    result += "     write";
  else
    result += "   none   ";

  // TODO - technically, we should determine if the label is read or write
  const string& l = debugger->cartDebug().getLabel(addr, true);
  if(l != "") {
    result += "  (";
    result += l;
    result += ")";
  }

  return result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool DebuggerParser::saveScriptFile(string file)
{
  if( file.find_last_of('.') == string::npos ) {
    file += ".stella";
  }

  ofstream out(file.c_str());

  FunctionDefMap funcs = debugger->getFunctionDefMap();
  for(FunctionDefMap::const_iterator i = funcs.begin(); i != funcs.end(); ++i)
    out << "function " << i->first << " { " << i->second << " }" << endl;

  for(unsigned int i=0; i<watches.size(); i++)
    out << "watch " << watches[i] << endl;

  for(unsigned int i=0; i<0x10000; i++)
    if(debugger->breakPoint(i))
      out << "break #" << i << endl;

  for(unsigned int i=0; i<0x10000; i++) {
    bool r = debugger->readTrap(i);
    bool w = debugger->writeTrap(i);

    if(r && w)
      out << "trap #" << i << endl;
    else if(r)
      out << "trapread #" << i << endl;
    else if(w)
      out << "trapwrite #" << i << endl;
  }

  StringList conds = debugger->cpuDebug().m6502().getCondBreakNames();
  for(unsigned int i=0; i<conds.size(); i++)
    out << "breakif {" << conds[i] << "}" << endl;

  bool ok = out.good();
  out.close();
  return ok;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// executor methods for commands[] array. All are void, no args.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "a"
void DebuggerParser::executeA()
{
  debugger->cpuDebug().setA((uInt8)args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "bank"	 
void DebuggerParser::executeBank()
{
  int banks = debugger->cartDebug().bankCount();
  if(argCount == 0)
  {
    commandResult << debugger->cartDebug().getCartType() << ": ";
    if(banks < 2)
      commandResult << red("bankswitching not supported by this cartridge");
    else
    {
      commandResult << "current = " << debugger->valueToString(debugger->cartDebug().getBank())
                    << " out of " << debugger->valueToString(banks) << " banks";
    }
  }
  else
  {
    if(banks == 1)
      commandResult << red("bankswitching not supported by this cartridge");
    else if(args[0] >= banks)
      commandResult << red("invalid bank number (must be 0 to ")
                    << debugger->valueToString(banks - 1) << ")";
    else if(debugger->setBank(args[0]))
      commandResult << "switched bank OK";
    else
      commandResult << red("error switching banks (bankswitching may not be supported)");
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "base"
void DebuggerParser::executeBase()
{
  if(args[0] == 2 || argStrings[0] == "bin")
    setBase(kBASE_2);
  else if(args[0] == 10 || argStrings[0] == "dec")
    setBase(kBASE_10);
  else if(args[0] == 16 || argStrings[0] == "hex")
    setBase(kBASE_16);

  commandResult << "default base set to ";
  switch(defaultBase) {
    case kBASE_2:
      commandResult << "#2/bin";
      break;

    case kBASE_10:
      commandResult << "#10/dec";
      break;

    case kBASE_16:
      commandResult << "#16/hex";
      break;

    default:
      commandResult << red("UNKNOWN");
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "break"
void DebuggerParser::executeBreak()
{
  int bp;
  if(argCount == 0)
    bp = debugger->cpuDebug().pc();
  else
    bp = args[0];
  debugger->toggleBreakPoint(bp);
  debugger->myRom->invalidate();

  if(debugger->breakPoint(bp))
    commandResult << "Set";
  else
    commandResult << "Cleared";

  commandResult << " breakpoint at " << debugger->valueToString(bp);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "breakif"
void DebuggerParser::executeBreakif()
{
  int res = YaccParser::parse(argStrings[0].c_str());
  if(res == 0)
  {
    uInt32 ret = debugger->cpuDebug().m6502().addCondBreak(
                 YaccParser::getResult(), argStrings[0] );
    commandResult << "Added breakif " << debugger->valueToString(ret);
  }
  else
    commandResult << red("invalid expression");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "c"
void DebuggerParser::executeC()
{
  if(argCount == 0)
    debugger->cpuDebug().toggleC();
  else if(argCount == 1)
    debugger->cpuDebug().setC(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "cheat"
// (see Stella manual for different cheat types)
void DebuggerParser::executeCheat()
{
#ifdef CHEATCODE_SUPPORT
  if(argCount == 0)
  {
    commandResult << red("Missing cheat code");
    return;
  }

  for(int arg = 0; arg < argCount; arg++)
  {
    const string& cheat = argStrings[arg];
    const Cheat* c = debugger->getOSystem()->cheat().add("DBG", cheat);
    if(c && c->enabled())
      commandResult << "Cheat code " << cheat << " enabled" << endl;
    else
      commandResult << red("Invalid cheat code ") << cheat << endl;
  }
#else
  commandResult << red("Cheat support not enabled\n");
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "clearbreaks"
void DebuggerParser::executeClearbreaks()
{
  debugger->clearAllBreakPoints();
  debugger->cpuDebug().m6502().clearCondBreaks();
  commandResult << "all breakpoints cleared";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "cleartraps"
void DebuggerParser::executeCleartraps()
{
  debugger->clearAllTraps();
  commandResult << "all traps cleared";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "clearwatches"
void DebuggerParser::executeClearwatches()
{
  watches.clear();
  commandResult << "all watches cleared";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "cls"
void DebuggerParser::executeCls()
{
  debugger->prompt()->clearScreen();
  commandResult << "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "colortest"
void DebuggerParser::executeColortest()
{
  commandResult << "test color: "
                << char((args[0]>>1) | 0x80)
                << inverse("        ");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "d"
void DebuggerParser::executeD()
{
  if(argCount == 0)
    debugger->cpuDebug().toggleD();
  else if(argCount == 1)
    debugger->cpuDebug().setD(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "define"
void DebuggerParser::executeDefine()
{
  // TODO: check if label already defined?
  debugger->cartDebug().addLabel(argStrings[0], args[1]);
  debugger->myRom->invalidate();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "delbreakif"
void DebuggerParser::executeDelbreakif()
{
  debugger->cpuDebug().m6502().delCondBreak(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "delfunction"
void DebuggerParser::executeDelfunction()
{
  if(debugger->delFunction(argStrings[0]))
    commandResult << "removed function " << argStrings[0];
  else
    commandResult << "function " << argStrings[0] << " not found";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "delwatch"
void DebuggerParser::executeDelwatch()
{
  int which = args[0] - 1;
  if(which >= 0 && which < (int)watches.size())
  {
    watches.remove_at(which);
    commandResult << "removed watch";
  }
  else
    commandResult << "no such watch";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "disasm"
void DebuggerParser::executeDisasm()
{
  int start, lines = 20;

  if(argCount == 0) {
    start = debugger->cpuDebug().pc();
  } else if(argCount == 1) {
    start = args[0];
  } else if(argCount == 2) {
    start = args[0];
    lines = args[1];
  } else {
    commandResult << "wrong number of arguments";
    return;
  }

  commandResult << debugger->cartDebug().disassemble(start, lines);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "dump"
void DebuggerParser::executeDump()
{
  for(int i=0; i<8; i++)
  {
    int start = args[0] + i*16;
    commandResult << debugger->valueToString(start) << ": ";
    for(int j=0; j<16; j++)
    {
      commandResult << debugger->valueToString(debugger->peek(start+j)) << " ";
      if(j == 7) commandResult << "- ";
    }
    if(i != 7) commandResult << endl;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "exec"
void DebuggerParser::executeExec()
{
  FilesystemNode file(argStrings[0]);
  commandResult << exec(file);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "help"
void DebuggerParser::executeHelp()
{
  // Find length of longest command
  uInt16 clen = 0;
  for(int i = 0; i < kNumCommands; ++i)
  {
    uInt16 len = commands[i].cmdString.length();
    if(len > clen)  clen = len;
  }

  for(int i = 0; i < kNumCommands; ++i)
    commandResult << setw(clen) << right << commands[i].cmdString
                  << " - " << commands[i].description << endl;

  commandResult << debugger->builtinHelp();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "frame"
void DebuggerParser::executeFrame()
{
  int count = 1;
  if(argCount != 0) count = args[0];
  debugger->nextFrame(count);
  commandResult << "advanced " << debugger->valueToString(count) << " frame";
  if(count != 1) commandResult << "s";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "function"
void DebuggerParser::executeFunction()
{
  if(args[0] >= 0)
  {
    commandResult << red("name already in use");
    return;
  }

  int res = YaccParser::parse(argStrings[1].c_str());
  if(res == 0)
  {
    debugger->addFunction(argStrings[0], argStrings[1], YaccParser::getResult());
    commandResult << "Added function " << argStrings[0] << " -> " << argStrings[1];
  }
  else
    commandResult << red("invalid expression");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "listbreaks"
void DebuggerParser::executeListbreaks()
{
  ostringstream buf;
  int count = 0;

  for(unsigned int i = 0; i < 0x10000; i++)
  {
    if(debugger->breakpoints().isSet(i))
    {
      buf << debugger->cartDebug().getLabel(i, true, 4) << " ";
      if(! (++count % 8) ) buf << "\n";
    }
  }

  /*
  if(count)
    return ret;
  else
    return "no breakpoints set";
    */
  if(count)
    commandResult << "breaks:\n" << buf.str();

  StringList conds = debugger->cpuDebug().m6502().getCondBreakNames();
  if(conds.size() > 0)
  {
    commandResult << "\nbreakifs:\n";
    for(unsigned int i=0; i<conds.size(); i++)
    {
      commandResult << debugger->valueToString(i) << ": " << conds[i];
      if(i != (conds.size() - 1)) commandResult << endl;
    }
  }

  if(commandResult.str() == "")
    commandResult << "no breakpoints set";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "listfunctions"
void DebuggerParser::executeListfunctions()
{
  const FunctionDefMap& functions = debugger->getFunctionDefMap();

  if(functions.size() > 0)
  {
    FunctionDefMap::const_iterator iter;
    for(iter = functions.begin(); iter != functions.end(); ++iter)
      commandResult << iter->first << " -> " << iter->second << endl;
  }
  else
    commandResult << "no user-defined functions";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "listtraps"
void DebuggerParser::executeListtraps()
{
  int count = 0;

  for(unsigned int i=0; i<0x10000; i++)
  {
    if(debugger->readTrap(i) || debugger->writeTrap(i))
    {
      commandResult << trapStatus(i) << endl;
      count++;
    }
  }

  if(!count)
    commandResult << "no traps set";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "loadstate"
void DebuggerParser::executeLoadstate()
{
  if(args[0] >= 0 && args[0] <= 9)
    debugger->loadState(args[0]);
  else
    commandResult << red("invalid slot (must be 0-9)");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "loadsym"
void DebuggerParser::executeLoadsym()
{
  commandResult << debugger->cartDebug().loadSymbolFile(argStrings[0]);
  debugger->myRom->invalidate();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "n"
void DebuggerParser::executeN()
{
  if(argCount == 0)
    debugger->cpuDebug().toggleN();
  else if(argCount == 1)
    debugger->cpuDebug().setN(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "pc"
void DebuggerParser::executePc()
{
  debugger->cpuDebug().setPC(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "print"
void DebuggerParser::executePrint()
{
  commandResult << eval();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "ram"
void DebuggerParser::executeRam()
{
  if(argCount == 0)
    commandResult << debugger->cartDebug().toString();
  else
    commandResult << debugger->setRAM(args);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "reset"
void DebuggerParser::executeReset()
{
  debugger->reset();
  debugger->myRom->invalidate();
  commandResult << "reset CPU";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "rewind"
void DebuggerParser::executeRewind()
{
  if(debugger->rewindState())
  {
    debugger->myRom->invalidate();
    commandResult << "rewind by one level";
  }
  else
    commandResult << "no states left to rewind";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "riot"
void DebuggerParser::executeRiot()
{
  commandResult << debugger->riotDebug().toString();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "rom"
void DebuggerParser::executeRom()
{
  int addr = args[0];
  for(int i=1; i<argCount; i++)
  {
    if( !(debugger->patchROM(addr++, args[i])) )
    {
      commandResult << red("patching ROM unsupported for this cart type");
      return;
    }
  }

  // Normally the run() method calls loadConfig() on the debugger,
  // which results in all child widgets being redrawn.
  // The RomWidget is a special case, since we don't want to re-disassemble
  // any more than necessary.  So we only do it by calling the following
  // method ...
  debugger->myRom->invalidate();

  commandResult << "changed " << debugger->valueToString( args.size() - 1 )
                << " location(s)";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "run"
void DebuggerParser::executeRun()
{
  debugger->saveOldState();
  debugger->quit();
  commandResult << "_EXIT_DEBUGGER";  // See PromptWidget for more info
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "runto"
void DebuggerParser::executeRunTo()
{
  const CartDebug& cartdbg = debugger->cartDebug();
  const CartDebug::DisassemblyList& list = cartdbg.disassembly().list;

  uInt32 count = 0, max_iterations = list.size();

  // Create a progress dialog box to show the progress searching through the
  // disassembly, since this may be a time-consuming operation
  ostringstream buf;
  buf << "RunTo searching through " << max_iterations << " disassembled instructions";
  ProgressDialog progress(debugger->myBaseDialog,
      debugger->getOSystem()->consoleFont(), buf.str());
  progress.setRange(0, max_iterations, 5);

  bool done = false;
  do {
    debugger->step();

    // Update romlist to point to current PC
    int pcline = cartdbg.addressToLine(debugger->cpuDebug().pc());
    if(pcline >= 0)
    {
      const string& next = list[pcline].disasm;
      done = (BSPF_findIgnoreCase(next, argStrings[0]) != string::npos);
    }
    // Update the progress bar
    progress.setProgress(count);
  } while(!done && ++count < max_iterations);

  progress.close();

  if(done)
    commandResult
      << "found " << argStrings[0] << " in "
      << debugger->valueToString(count, kBASE_10)
      << " disassembled instructions";
  else
    commandResult
      << argStrings[0] << " not found in "
      << debugger->valueToString(count, kBASE_10)
      << " disassembled instructions";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "runtopc"
void DebuggerParser::executeRunToPc()
{
  const CartDebug& cartdbg = debugger->cartDebug();
  const CartDebug::DisassemblyList& list = cartdbg.disassembly().list;

  uInt32 count = 0;
  bool done = false;
  do {
    debugger->step();

    // Update romlist to point to current PC
    int pcline = cartdbg.addressToLine(debugger->cpuDebug().pc());
    done = (pcline >= 0) && (list[pcline].address == args[0]);
    ++count;
  } while(!done && count < list.size());

  if(done)
    commandResult
      << "set PC to " << hex << args[0] << " in "
      << debugger->valueToString(count, kBASE_10)
      << " disassembled instructions";
  else
    commandResult
      << "PC " << hex << args[0] << " not reached or found in "
      << debugger->valueToString(count, kBASE_10)
      << " disassembled instructions";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "s"
void DebuggerParser::executeS()
{
  debugger->cpuDebug().setSP((uInt8)args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "save"
void DebuggerParser::executeSave()
{
  if(saveScriptFile(argStrings[0]))
    commandResult << "saved script to file " << argStrings[0];
  else
    commandResult << red("I/O error");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "saverom"
void DebuggerParser::executeSaverom()
{
  if(debugger->saveROM(argStrings[0]))
    commandResult << "saved ROM as " << argStrings[0];
  else
    commandResult << red("failed to save ROM");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "saveses"
void DebuggerParser::executeSaveses()
{
  if(debugger->prompt()->saveBuffer(argStrings[0]))
    commandResult << "saved session to file " << argStrings[0];
  else
    commandResult << red("I/O error");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "savestate"
void DebuggerParser::executeSavestate()
{
  if(args[0] >= 0 && args[0] <= 9)
    debugger->saveState(args[0]);
  else
    commandResult << red("invalid slot (must be 0-9)");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "scanline"
void DebuggerParser::executeScanline()
{
  int count = 1;
  if(argCount != 0) count = args[0];
  debugger->nextScanline(count);
  commandResult << "advanced " << debugger->valueToString(count) << " scanline";
  if(count != 1) commandResult << "s";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "step"
void DebuggerParser::executeStep()
{
  commandResult
    << "executed " << debugger->valueToString(debugger->step()) << " cycles";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "tia"
void DebuggerParser::executeTia()
{
  commandResult << debugger->tiaDebug().toString();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "trace"
void DebuggerParser::executeTrace()
{
  commandResult
    << "executed " << debugger->valueToString(debugger->trace()) << " cycles";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "trap"
void DebuggerParser::executeTrap()
{
  debugger->toggleReadTrap(args[0]);
  debugger->toggleWriteTrap(args[0]);
  commandResult << trapStatus(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "trapread"
void DebuggerParser::executeTrapread()
{
  debugger->toggleReadTrap(args[0]);
  commandResult << trapStatus(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "trapwrite"
void DebuggerParser::executeTrapwrite()
{
  debugger->toggleWriteTrap(args[0]);
  commandResult << trapStatus(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "undef"
void DebuggerParser::executeUndef()
{
  if(debugger->cartDebug().removeLabel(argStrings[0]))
  {
    debugger->myRom->invalidate();
    commandResult << argStrings[0] + " now undefined";
  }
  else
    commandResult << red("no such label");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "v"
void DebuggerParser::executeV()
{
  if(argCount == 0)
    debugger->cpuDebug().toggleV();
  else if(argCount == 1)
    debugger->cpuDebug().setV(args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "watch"
void DebuggerParser::executeWatch()
{
  watches.push_back(argStrings[0]);
  commandResult << "added watch \"" << argStrings[0] << "\"";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "x"
void DebuggerParser::executeX()
{
  debugger->cpuDebug().setX((uInt8)args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "y"
void DebuggerParser::executeY()
{
  debugger->cpuDebug().setY((uInt8)args[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// "z"
void DebuggerParser::executeZ()
{
  if(argCount == 0)
    debugger->cpuDebug().toggleZ();
  else if(argCount == 1)
    debugger->cpuDebug().setZ(args[0]);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// List of all commands available to the parser
DebuggerParser::Command DebuggerParser::commands[kNumCommands] = {
  {
    "a",
    "Set Accumulator to value xx",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeA
  },

  {
    "bank",
    "Show # of banks, or switch to bank xx",
    false,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeBank
  },

  {
    "base",
    "Set default base (hex, dec, or bin)",
    true,
    true,
    { kARG_BASE_SPCL, kARG_END_ARGS },
    &DebuggerParser::executeBase
  },

  {
    "break",
    "Set/clear breakpoint at address xx (default=PC)",
    false,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeBreak
  },

  {
    "breakif",
    "Set breakpoint on condition xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeBreakif
  },

  {
    "c",
    "Carry Flag: set (0 or 1), or toggle (no arg)",
    false,
    true,
    { kARG_BOOL, kARG_END_ARGS },
    &DebuggerParser::executeC
  },

  {
    "cheat",
    "Use a cheat code (see manual for cheat types)",
    false,
    false,
    { kARG_LABEL, kARG_END_ARGS },
    &DebuggerParser::executeCheat
  },

  {
    "clearbreaks",
    "Clear all breakpoints",
    false,
    true,
    { kARG_END_ARGS },
    &DebuggerParser::executeClearbreaks
  },

  {
    "cleartraps",
    "Clear all traps",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeCleartraps
  },

  {
    "clearwatches",
    "Clear all watches",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeClearwatches
  },

  {
    "cls",
    "Clear prompt area of text and erase history",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeCls
  },

  {
    "colortest",
    "Show value xx as TIA color",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeColortest
  },

  {
    "d",
    "Decimal Flag: set (0 or 1), or toggle (no arg)",
    false,
    true,
    { kARG_BOOL, kARG_END_ARGS },
    &DebuggerParser::executeD
  },

  {
    "define",
    "Define label xx for address yy",
    true,
    true,
    { kARG_LABEL, kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeDefine
  },

  {
    "delbreakif",
    "Delete conditional breakif xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeDelbreakif
  },

  {
    "delfunction",
    "Delete function with label xx",
    true,
    false,
    { kARG_LABEL, kARG_END_ARGS },
    &DebuggerParser::executeDelfunction
  },

  {
    "delwatch",
    "Delete watch xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeDelwatch
  },

  {
    "disasm",
    "Disassemble address xx [yy lines] (default=PC)",
    false,
    false,
    { kARG_WORD, kARG_MULTI_BYTE },
    &DebuggerParser::executeDisasm
  },

  {
    "dump",
    "Dump 128 bytes of memory at address xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeDump
  },

  {
    "exec",
    "Execute script file xx",
    true,
    true,
    { kARG_FILE, kARG_END_ARGS },
    &DebuggerParser::executeExec
  },

  {
    "frame",
    "Advance emulation by xx frames (default=1)",
    false,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeFrame
  },

  {
    "function",
    "Define function name xx for expression yy",
    true,
    false,
    { kARG_LABEL, kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeFunction
  },

  {
    "help",
    "This cruft",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeHelp
  },

  {
    "listbreaks",
    "List breakpoints",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeListbreaks
  },

  {
    "listfunctions",
    "List user-defined functions",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeListfunctions
  },

  {
    "listtraps",
    "List traps",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeListtraps
  },

  {
    "loadstate",
    "Load emulator state xx (0-9)",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeLoadstate
  },

  {
    "loadsym",
    "Load symbol file named xx",
    true,
    true,
    { kARG_FILE, kARG_END_ARGS },
    &DebuggerParser::executeLoadsym
  },

  {
    "n",
    "Negative Flag: set (0 or 1), or toggle (no arg)",
    false,
    true,
    { kARG_BOOL, kARG_END_ARGS },
    &DebuggerParser::executeN
  },

  {
    "pc",
    "Set Program Counter to address xx",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executePc
  },

  {
    "print",
    "Evaluate/print expression xx in hex/dec/binary",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executePrint
  },

  {
    "ram",
    "Show ZP RAM, or set address xx to yy1 [yy2 ...]",
    false,
    true,
    { kARG_WORD, kARG_MULTI_BYTE },
    &DebuggerParser::executeRam
  },

  {
    "reset",
    "Reset 6507 to init vector (excluding TIA/RIOT)",
    false,
    true,
    { kARG_END_ARGS },
    &DebuggerParser::executeReset
  },

  {
    "rewind",
    "Rewind state to last step/trace/scanline/frame",
    false,
    true,
    { kARG_END_ARGS },
    &DebuggerParser::executeRewind
  },

  {
    "riot",
    "Show RIOT timer/input status",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeRiot
  },

  {
    "rom",
    "Set ROM address xx to yy1 [yy2 ...]",
    true,
    true,
    { kARG_WORD, kARG_MULTI_BYTE },
    &DebuggerParser::executeRom
  },

  {
    "run",
    "Exit debugger, return to emulator",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeRun
  },

  {
    "runto",
    "Run until string xx in disassembly",
    true,
    true,
    { kARG_LABEL, kARG_END_ARGS },
    &DebuggerParser::executeRunTo
  },

  {
    "runtopc",
    "Run until PC is set to value xx",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeRunToPc
  },

  {
    "s",
    "Set Stack Pointer to value xx",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeS
  },

  {
    "save",
    "Save breaks, watches, traps to file xx",
    true,
    false,
    { kARG_FILE, kARG_END_ARGS },
    &DebuggerParser::executeSave
  },

  {
    "saverom",
    "Save (possibly patched) ROM to file xx",
    true,
    false,
    { kARG_FILE, kARG_END_ARGS },
    &DebuggerParser::executeSaverom
  },

  {
    "saveses",
    "Save console session to file xx",
    true,
    false,
    { kARG_FILE, kARG_END_ARGS },
    &DebuggerParser::executeSaveses
  },

  {
    "savestate",
    "Save emulator state xx (valid args 0-9)",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeSavestate
  },

  {
    "scanline",
    "Advance emulation by xx scanlines (default=1)",
    false,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeScanline
  },

  {
    "step",
    "Single step CPU [with count xx]",
    false,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeStep
  },

  {
    "tia",
    "Show TIA state (NOT FINISHED YET)",
    false,
    false,
    { kARG_END_ARGS },
    &DebuggerParser::executeTia
  },

  {
    "trace",
    "Single step CPU over subroutines [with count xx]",
    false,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeTrace
  },

  {
    "trap",
    "Trap read and write accesses to address xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeTrap
  },

  {
    "trapread",
    "Trap read accesses to address xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeTrapread
  },

  {
    "trapwrite",
    "Trap write accesses to address xx",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeTrapwrite
  },

  {
    "undef",
    "Undefine label xx (if defined)",
    true,
    true,
    { kARG_LABEL, kARG_END_ARGS },
    &DebuggerParser::executeUndef
  },

  {
    "v",
    "Overflow Flag: set (0 or 1), or toggle (no arg)",
    false,
    true,
    { kARG_BOOL, kARG_END_ARGS },
    &DebuggerParser::executeV
  },

  {
    "watch",
    "Print contents of address xx before every prompt",
    true,
    false,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeWatch
  },

  {
    "x",
    "Set X Register to value xx",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeX
  },

  {
    "y",
    "Set Y Register to value xx",
    true,
    true,
    { kARG_WORD, kARG_END_ARGS },
    &DebuggerParser::executeY
  },

  {
    "z",
    "Zero Flag: set (0 or 1), or toggle (no arg)",
    false,
    true,
    { kARG_BOOL, kARG_END_ARGS },
    &DebuggerParser::executeZ
  }
};
