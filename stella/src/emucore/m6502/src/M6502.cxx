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
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#include "M6502.hxx"

#ifdef DEBUGGER_SUPPORT
  #include "Expression.hxx"
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502::M6502(uInt32 systemCyclesPerProcessorCycle)
  : myExecutionStatus(0),
    mySystem(0),
    mySystemCyclesPerProcessorCycle(systemCyclesPerProcessorCycle),
    myLastAccessWasRead(true),
    myTotalInstructionCount(0)
{
#ifdef DEBUGGER_SUPPORT
  myDebugger    = NULL;
  myBreakPoints = NULL;
  myReadTraps   = NULL;
  myWriteTraps  = NULL;
#endif

  // Compute the System Cycle table
  for(uInt32 t = 0; t < 256; ++t)
  {
    myInstructionSystemCycleTable[t] = ourInstructionProcessorCycleTable[t] *
        mySystemCyclesPerProcessorCycle;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502::~M6502()
{
#ifdef DEBUGGER_SUPPORT
  myBreakConds.clear();
  myBreakCondNames.clear();
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::install(System& system)
{
  // Remember which system I'm installed in
  mySystem = &system;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::reset()
{
  // Clear the execution status flags
  myExecutionStatus = 0;

  // Set registers to default values
  A = X = Y = 0;
  SP = 0xff;
  PS(0x20);

  // Reset access flag
  myLastAccessWasRead = true;

  // Load PC from the reset vector
  PC = (uInt16)mySystem->peek(0xfffc) | ((uInt16)mySystem->peek(0xfffd) << 8);

  myTotalInstructionCount = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::irq()
{
  myExecutionStatus |= MaskableInterruptBit;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::nmi()
{
  myExecutionStatus |= NonmaskableInterruptBit;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::stop()
{
  myExecutionStatus |= StopExecutionBit;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt8 M6502::PS() const
{
  uInt8 ps = 0x20;

  if(N) 
    ps |= 0x80;
  if(V) 
    ps |= 0x40;
  if(B) 
    ps |= 0x10;
  if(D) 
    ps |= 0x08;
  if(I) 
    ps |= 0x04;
  if(!notZ) 
    ps |= 0x02;
  if(C) 
    ps |= 0x01;

  return ps;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::PS(uInt8 ps)
{
  N = ps & 0x80;
  V = ps & 0x40;
  B = true;        // B = ps & 0x10;  The 6507's B flag always true
  D = ps & 0x08;
  I = ps & 0x04;
  notZ = !(ps & 0x02);
  C = ps & 0x01;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream& out, const M6502::AddressingMode& mode)
{
  switch(mode)
  {
    case M6502::Absolute:
      out << "$nnnn  ";
      break;
    case M6502::AbsoluteX:
      out << "$nnnn,X";
      break;
    case M6502::AbsoluteY:
      out << "$nnnn,Y";
      break;
    case M6502::Implied:
      out << "implied";
      break;
    case M6502::Immediate:
      out << "#$nn   ";
      break;
    case M6502::Indirect:
      out << "($nnnn)";
      break;
    case M6502::IndirectX:
      out << "($nn,X)";
      break;
    case M6502::IndirectY:
      out << "($nn),Y";
      break;
    case M6502::Invalid:
      out << "invalid";
      break;
    case M6502::Relative:
      out << "$nn    ";
      break;
    case M6502::Zero:
      out << "$nn    ";
      break;
    case M6502::ZeroX:
      out << "$nn,X  ";
      break;
    case M6502::ZeroY:
      out << "$nn,Y  ";
      break;
  }
  return out;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502::AddressingMode M6502::ourAddressingModeTable[256] = {
  Implied,    IndirectX, Invalid,   IndirectX,    // 0x0?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0x1?
  ZeroX,      ZeroX,     ZeroX,     ZeroX,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteX, AbsoluteX,

  Absolute,   IndirectX, Invalid,   IndirectX,    // 0x2?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0x3?
  ZeroX,      ZeroX,     ZeroX,     ZeroX,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteX, AbsoluteX,

  Implied,    IndirectX, Invalid,   IndirectX,    // 0x4?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0x5?
  ZeroX,      ZeroX,     ZeroX,     ZeroX,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteX, AbsoluteX,

  Implied,    IndirectX, Invalid,   IndirectX,    // 0x6?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Indirect,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0x7?
  ZeroX,      ZeroX,     ZeroX,     ZeroX,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteX, AbsoluteX,

  Immediate,  IndirectX, Immediate, IndirectX,    // 0x8?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0x9?
  ZeroX,      ZeroX,     ZeroY,     ZeroY,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteY, AbsoluteY,

  Immediate,  IndirectX, Immediate, IndirectX,    // 0xA?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0xB?
  ZeroX,      ZeroX,     ZeroY,     ZeroY,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteY, AbsoluteY,

  Immediate,  IndirectX, Immediate, IndirectX,    // 0xC?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0xD?
  ZeroX,      ZeroX,     ZeroX,     ZeroX,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteX, AbsoluteX,

  Immediate,  IndirectX, Immediate, IndirectX,    // 0xE?
  Zero,       Zero,      Zero,      Zero,
  Implied,    Immediate, Implied,   Immediate,
  Absolute,   Absolute,  Absolute,  Absolute,

  Relative,   IndirectY, Invalid,   IndirectY,    // 0xF?
  ZeroX,      ZeroX,     ZeroX,     ZeroX,
  Implied,    AbsoluteY, Implied,   AbsoluteY,
  AbsoluteX,  AbsoluteX, AbsoluteX, AbsoluteX
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
M6502::AccessMode M6502::ourAccessModeTable[256] = {
  None,   Read,   None,   Write,    // 0x0?
  None,   Read,   Write,  Write,
  None,   Read,   Write,  Read,
  None,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0x1?
  None,   Read,   Write,  Write,
  None,   Read,   None,   Write,
  None,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0x2?
  Read,   Read,   Write,  Write,
  None,   Read,   Write,  Read,
  Read,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0x3?
  None,   Read,   Write,  Write,
  None,   Read,   None,   Write,
  None,   Read,   Write,  Write,
  
  None,   Read,   None,   Write,    // 0x4?
  None,   Read,   Write,  Write,
  None,   Read,   Write,  Read,
  Read,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0x5?
  None,   Read,   Write,  Write,
  None,   Read,   None,   Write,
  None,   Read,   Write,  Write,

  None,   Read,   None,   Write,    // 0x6?
  None,   Read,   Write,  Write,
  None,   Read,   Write,  Read,
  Read,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0x7?
  None,   Read,   Write,  Write,
  None,   Read,   None,   Write,
  None,   Read,   Write,  Write,

  None,   Write,  None,   Write,    // 0x8?
  Write,  Write,  Write,  Write,
  None,   None,   None,   Read,
  Write,  Write,  Write,  Write,

  Read,   Write,  None,   Write,    // 0x9?
  Write,  Write,  Write,  Write,
  None,   Write,  None,   Write,
  Write,  Write,  Write,  Write,

  Read,   Read,   Read,   Read,     // 0xA?
  Read,   Read,   Read,   Read,
  None,   Read,   None,   Read,
  Read,   Read,   Read,   Read,

  Read,   Read,   None,   Read,     // 0xB?
  Read,   Read,   Read,   Read,
  None,   Read,   None,   Read,
  Read,   Read,   Read,   Read,

  Read,   Read,   None,   Write,    // 0xC?
  Read,   Read,   Write,  Write,
  None,   Read,   None,   Read,
  Read,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0xD?
  None,   Read,   Write,  Write,
  None,   Read,   None,   Write,
  None,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0xE?
  Read,   Read,   Write,  Write,
  None,   Read,   None,   Read,
  Read,   Read,   Write,  Write,

  Read,   Read,   None,   Write,    // 0xF?
  None,   Read,   Write,  Write,
  None,   Read,   None,   Write,
  None,   Read,   Write,  Write
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 M6502::ourInstructionProcessorCycleTable[256] = {
//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,  // 0
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,  // 1
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,  // 2
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,  // 3
    6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,  // 4
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,  // 5
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,  // 6
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,  // 7
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,  // 8
    2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,  // 9
    2, 6, 2, 6, 3, 3, 3, 4, 2, 2, 2, 2, 4, 4, 4, 4,  // a
    2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,  // b
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,  // c
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,  // d
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,  // e
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7   // f
  };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* M6502::ourInstructionMnemonicTable[256] = {
  "BRK",  "ORA",  "n/a",  "slo",  "nop",  "ORA",  "ASL",  "slo",    // 0x0?
  "PHP",  "ORA",  "ASLA", "anc",  "nop",  "ORA",  "ASL",  "slo",

  "BPL",  "ORA",  "n/a",  "slo",  "nop",  "ORA",  "ASL",  "slo",    // 0x1?
  "CLC",  "ORA",  "nop",  "slo",  "nop",  "ORA",  "ASL",  "slo",

  "JSR",  "AND",  "n/a",  "rla",  "BIT",  "AND",  "ROL",  "rla",    // 0x2?
  "PLP",  "AND",  "ROLA", "anc",  "BIT",  "AND",  "ROL",  "rla",

  "BMI",  "AND",  "n/a",  "rla",  "nop",  "AND",  "ROL",  "rla",    // 0x3?
  "SEC",  "AND",  "nop",  "rla",  "nop",  "AND",  "ROL",  "rla",
  
  "RTI",  "EOR",  "n/a",  "sre",  "nop",  "EOR",  "LSR",  "sre",    // 0x4?
  "PHA",  "EOR",  "LSRA", "asr",  "JMP",  "EOR",  "LSR",  "sre",

  "BVC",  "EOR",  "n/a",  "sre",  "nop",  "EOR",  "LSR",  "sre",    // 0x5?
  "CLI",  "EOR",  "nop",  "sre",  "nop",  "EOR",  "LSR",  "sre",

  "RTS",  "ADC",  "n/a",  "rra",  "nop",  "ADC",  "ROR",  "rra",    // 0x6?
  "PLA",  "ADC",  "RORA", "arr",  "JMP",  "ADC",  "ROR",  "rra",

  "BVS",  "ADC",  "n/a",  "rra",  "nop",  "ADC",  "ROR",  "rra",    // 0x7?
  "SEI",  "ADC",  "nop",  "rra",  "nop",  "ADC",  "ROR",  "rra",

  "nop",  "STA",  "nop",  "sax",  "STY",  "STA",  "STX",  "sax",    // 0x8?
  "DEY",  "nop",  "TXA",  "ane",  "STY",  "STA",  "STX",  "sax",

  "BCC",  "STA",  "n/a",  "sha",  "STY",  "STA",  "STX",  "sax",    // 0x9?
  "TYA",  "STA",  "TXS",  "shs",  "shy",  "STA",  "shx",  "sha",

  "LDY",  "LDA",  "LDX",  "lax",  "LDY",  "LDA",  "LDX",  "lax",    // 0xA?
  "TAY",  "LDA",  "TAX",  "lxa",  "LDY",  "LDA",  "LDX",  "lax",

  "BCS",  "LDA",  "n/a",  "lax",  "LDY",  "LDA",  "LDX",  "lax",    // 0xB?
  "CLV",  "LDA",  "TSX",  "las",  "LDY",  "LDA",  "LDX",  "lax",

  "CPY",  "CMP",  "nop",  "dcp",  "CPY",  "CMP",  "DEC",  "dcp",    // 0xC?
  "INY",  "CMP",  "DEX",  "sbx",  "CPY",  "CMP",  "DEC",  "dcp",

  "BNE",  "CMP",  "n/a",  "dcp",  "nop",  "CMP",  "DEC",  "dcp",    // 0xD?
  "CLD",  "CMP",  "nop",  "dcp",  "nop",  "CMP",  "DEC",  "dcp",

  "CPX",  "SBC",  "nop",  "isb",  "CPX",  "SBC",  "INC",  "isb",    // 0xE?
  "INX",  "SBC",  "NOP",  "sbc",  "CPX",  "SBC",  "INC",  "isb",

  "BEQ",  "SBC",  "n/a",  "isb",  "nop",  "SBC",  "INC",  "isb",    // 0xF?
  "SED",  "SBC",  "nop",  "isb",  "nop",  "SBC",  "INC",  "isb"
};

#ifdef DEBUGGER_SUPPORT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::attach(Debugger& debugger)
{
  // Remember the debugger for this microprocessor
  myDebugger = &debugger;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned int M6502::addCondBreak(Expression *e, const string& name)
{
  myBreakConds.push_back(e);
  myBreakCondNames.push_back(name);
  return myBreakConds.size() - 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::delCondBreak(unsigned int brk)
{
  if(brk < myBreakConds.size())
  {
    delete myBreakConds[brk];
    myBreakConds.remove_at(brk);
    myBreakCondNames.remove_at(brk);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::clearCondBreaks()
{
  for(uInt32 i = 0; i < myBreakConds.size(); i++)
    delete myBreakConds[i];

  myBreakConds.clear();
  myBreakCondNames.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const StringList& M6502::getCondBreakNames() const
{
  return myBreakCondNames;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int M6502::evalCondBreaks()
{
  for(uInt32 i = 0; i < myBreakConds.size(); i++)
    if(myBreakConds[i]->evaluate())
      return i;

  return -1; // no break hit
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::setBreakPoints(PackedBitArray *bp)
{
  myBreakPoints = bp;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void M6502::setTraps(PackedBitArray *read, PackedBitArray *write)
{
  myReadTraps = read;
  myWriteTraps = write;
}

#endif
