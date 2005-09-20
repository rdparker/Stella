//============================================================================
//
// MM     MM  6666  555555  0000   2222
// MMMM MMMM 66  66 55     00  00 22  22
// MM MMM MM 66     55     00  00     22
// MM  M  MM 66666  55555  00  00  22222  --  "A 6502 Microprocessor Emulator"
// MM     MM 66  66     55 00  00 22
// MM     MM 66  66 55  55 00  00 22
// MM     MM  6666   5555   0000  222222
//
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: M6502Hi.cxx,v 1.13 2005-09-20 19:09:10 stephena Exp $
//============================================================================

#include "M6502Hi.hxx"
#include "Serializer.hxx"
#include "Deserializer.hxx"

#ifdef DEVELOPER_SUPPORT
  #include "Debugger.hxx"
#endif

#define debugStream cout

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502High::M6502High(uInt32 systemCyclesPerProcessorCycle)
    : M6502(systemCyclesPerProcessorCycle)
{
  myNumberOfDistinctAccesses = 0;
  myLastAddress = 0;

#ifdef DEVELOPER_SUPPORT
  myJustHitTrapFlag = false;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502High::~M6502High()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline uInt8 M6502High::peek(uInt16 address)
{
  if(address != myLastAddress)
  {
    myNumberOfDistinctAccesses++;
    myLastAddress = address;
  }
  mySystem->incrementCycles(mySystemCyclesPerProcessorCycle);

#ifdef DEVELOPER_SUPPORT
  if(myReadTraps != NULL && myReadTraps->isSet(address))
  {
    myJustHitTrapFlag = true;
    myHitTrapInfo.message = "Read trap: ";
    myHitTrapInfo.address = address;
  }
#endif

  return mySystem->peek(address);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline void M6502High::poke(uInt16 address, uInt8 value)
{
  if(address != myLastAddress)
  {
    myNumberOfDistinctAccesses++;
    myLastAddress = address;
  }
  mySystem->incrementCycles(mySystemCyclesPerProcessorCycle);

#ifdef DEVELOPER_SUPPORT
  if(myWriteTraps != NULL && myWriteTraps->isSet(address))
  {
    myJustHitTrapFlag = true;
    myHitTrapInfo.message = "Write trap: ";
    myHitTrapInfo.address = address;
  }
#endif

  mySystem->poke(address, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6502High::execute(uInt32 number)
{
  int cond = -1;

  // Clear all of the execution status bits except for the fatal error bit
  myExecutionStatus &= FatalErrorBit;

  // Loop until execution is stopped or a fatal error occurs
  for(;;)
  {
    for(; !myExecutionStatus && (number != 0); --number)
    {
      uInt16 operandAddress = 0;
      uInt8 operand = 0;

#ifdef DEVELOPER_SUPPORT
      if(myJustHitTrapFlag)
      {
        if(myDebugger->start(myHitTrapInfo.message, myHitTrapInfo.address))
        {
          myJustHitTrapFlag = false;
          return true;
        }
      }

      if(myBreakPoints != NULL)
      {
        if(myBreakPoints->isSet(PC))
        {
          if(myDebugger->start("Breakpoint hit: ", PC))
            return true;
        }
      }

      if((cond = evalCondBreaks()) > -1)
      {
        string buf = "CBP: " + myBreakCondNames[cond];
        if(myDebugger->start(buf))
          return true;
      }
#endif

#ifdef DEBUG
      debugStream << "PC=" << hex << setw(4) << PC << " ";
#endif

      // Fetch instruction at the program counter
      IR = peek(PC++);

#ifdef DEBUG
      debugStream << "IR=" << hex << setw(2) << (int)IR << " ";
      debugStream << "<" << ourAddressingModeTable[IR] << " ";
#endif

      // Call code to execute the instruction
      switch(IR)
      {
        // 6502 instruction emulation is generated by an M4 macro file
        #include "M6502Hi.ins"

        default:
          // Oops, illegal instruction executed so set fatal error flag
          myExecutionStatus |= FatalErrorBit;
      }

      myTotalInstructionCount++;

#ifdef DEBUG
      debugStream << hex << setw(4) << operandAddress << " ";
      debugStream << setw(4) << ourInstructionMnemonicTable[IR];
      debugStream << "> ";
      debugStream << "A=" << ::hex << setw(2) << (int)A << " ";
      debugStream << "X=" << ::hex << setw(2) << (int)X << " ";
      debugStream << "Y=" << ::hex << setw(2) << (int)Y << " ";
      debugStream << "PS=" << ::hex << setw(2) << (int)PS() << " ";
      debugStream << "SP=" << ::hex << setw(2) << (int)SP << " ";
      debugStream << "Cyc=" << dec << mySystem->cycles();
      debugStream << endl;
#endif
    }

    // See if we need to handle an interrupt
    if((myExecutionStatus & MaskableInterruptBit) || 
        (myExecutionStatus & NonmaskableInterruptBit))
    {
      // Yes, so handle the interrupt
      interruptHandler();
    }

    // See if execution has been stopped
    if(myExecutionStatus & StopExecutionBit)
    {
      // Yes, so answer that everything finished fine
      return true;
    }

    // See if a fatal error has occured
    if(myExecutionStatus & FatalErrorBit)
    {
      // Yes, so answer that something when wrong
      return false;
    }

    // See if we've executed the specified number of instructions
    if(number == 0)
    {
      // Yes, so answer that everything finished fine
      return true;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502High::interruptHandler()
{
  // Handle the interrupt
  if((myExecutionStatus & MaskableInterruptBit) && !I)
  {
    mySystem->incrementCycles(7 * mySystemCyclesPerProcessorCycle);
    mySystem->poke(0x0100 + SP--, (PC - 1) >> 8);
    mySystem->poke(0x0100 + SP--, (PC - 1) & 0x00ff);
    mySystem->poke(0x0100 + SP--, PS() & (~0x10));
    D = false;
    I = true;
    PC = (uInt16)mySystem->peek(0xFFFE) | ((uInt16)mySystem->peek(0xFFFF) << 8);
  }
  else if(myExecutionStatus & NonmaskableInterruptBit)
  {
    mySystem->incrementCycles(7 * mySystemCyclesPerProcessorCycle);
    mySystem->poke(0x0100 + SP--, (PC - 1) >> 8);
    mySystem->poke(0x0100 + SP--, (PC - 1) & 0x00ff);
    mySystem->poke(0x0100 + SP--, PS() & (~0x10));
    D = false;
    PC = (uInt16)mySystem->peek(0xFFFA) | ((uInt16)mySystem->peek(0xFFFB) << 8);
  }

  // Clear the interrupt bits in myExecutionStatus
  myExecutionStatus &= ~(MaskableInterruptBit | NonmaskableInterruptBit);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6502High::save(Serializer& out)
{
  string CPU = name();

  try
  {
    out.putString(CPU);

    out.putLong(A);    // Accumulator
    out.putLong(X);    // X index register
    out.putLong(Y);    // Y index register
    out.putLong(SP);   // Stack Pointer
    out.putLong(IR);   // Instruction register
    out.putLong(PC);   // Program Counter

    out.putBool(N);     // N flag for processor status register
    out.putBool(V);     // V flag for processor status register
    out.putBool(B);     // B flag for processor status register
    out.putBool(D);     // D flag for processor status register
    out.putBool(I);     // I flag for processor status register
    out.putBool(notZ);  // Z flag complement for processor status register
    out.putBool(C);     // C flag for processor status register

    out.putLong(myExecutionStatus);

    // Indicates the number of distinct memory accesses
    out.putLong(myNumberOfDistinctAccesses);
    // Indicates the last address which was accessed
    out.putLong(myLastAddress);

  }
  catch(char *msg)
  {
    cerr << msg << endl;
    return false;
  }
  catch(...)
  {
    cerr << "Unknown error in save state for " << CPU << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool M6502High::load(Deserializer& in)
{
  string CPU = name();

  try
  {
    if(in.getString() != CPU)
      return false;

    A = (uInt8) in.getLong();    // Accumulator
    X = (uInt8) in.getLong();    // X index register
    Y = (uInt8) in.getLong();    // Y index register
    SP = (uInt8) in.getLong();   // Stack Pointer
    IR = (uInt8) in.getLong();   // Instruction register
    PC = (uInt16) in.getLong();  // Program Counter

    N = in.getBool();     // N flag for processor status register
    V = in.getBool();     // V flag for processor status register
    B = in.getBool();     // B flag for processor status register
    D = in.getBool();     // D flag for processor status register
    I = in.getBool();     // I flag for processor status register
    notZ = in.getBool();  // Z flag complement for processor status register
    C = in.getBool();     // C flag for processor status register

    myExecutionStatus = (uInt8) in.getLong();

    // Indicates the number of distinct memory accesses
    myNumberOfDistinctAccesses = (uInt32) in.getLong();
    // Indicates the last address which was accessed
    myLastAddress = (uInt16) in.getLong();
  }
  catch(char *msg)
  {
    cerr << msg << endl;
    return false;
  }
  catch(...)
  {
    cerr << "Unknown error in load state for " << CPU << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* M6502High::name() const
{
  return "M6502High";
}
