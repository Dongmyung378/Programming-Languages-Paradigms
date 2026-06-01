#pragma once

// modified by Dongmyung

// basic libraries are used in this file
#include "common.h"
#include <string>

// Enum representing the various opcodes
enum InstructionOpcode
{
  ADD = 0,
  AND,
  ORR,
  XOR,
  LDR,
  STR,
  JMP,
  JNE,
  NUM_OPCODES
};

//------------------------------------------------------------------------------
//--------------------        INSTRUCTION SUBCLASSES        --------------------
//------------------------------------------------------------------------------

// Class representing an ADD instruction
class Iadd : public InstructionBase
{
public:
  explicit Iadd(addr_t address);
  // write overide to make compiler checks if the function you want to override actually exists in the base class
  // same reason to below classes
  void _execute(ProcessorState &state) const override;
  // replace char* to std::string to manage resource
  // same reason to below classes
  std::string name() const override;
};

// Class representing an AND instruction
class Iand : public InstructionBase
{
public:
  explicit Iand(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};

// Class representing an ORR instruction
class Iorr : public InstructionBase
{
public:
  explicit Iorr(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};

// Class representing an XOR instruction
class Ixor : public InstructionBase
{
public:
  explicit Ixor(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};

// Class representing an LDR instruction
class Ildr : public InstructionBase
{
public:
  explicit Ildr(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};

// Class representing an STR instruction
class Istr : public InstructionBase
{
public:
  explicit Istr(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};

// Class representing an JMP instruction
class Ijmp : public InstructionBase
{
public:
  explicit Ijmp(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};

// Class representing an JNE instruction
class Ijne : public InstructionBase
{
public:
  explicit Ijne(addr_t address);
  void _execute(ProcessorState &state) const override;
  std::string name() const override;
};
