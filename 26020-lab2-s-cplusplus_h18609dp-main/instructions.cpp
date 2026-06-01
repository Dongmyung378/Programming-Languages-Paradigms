// modified by Dongmyung

// basic libraries are used in this file
#include <cassert>
#include <sstream>
#include <memory>
#include "instructions.h"

// ========== InstructionBase ==========
void InstructionBase::execute(ProcessorState &state) const
{
  // virtual call that implements the actual functionality of the instruction
  _execute(state);

  // move the pc forward
  state.pc += INSTRUCTION_SIZE;

  // trim the accumulator and the PC to fit in number of bits of the architecture
  state.acc &= ARCH_BITMASK;
  state.pc &= ARCH_BITMASK;
}

addr_t InstructionBase::get_address() const
{
  return _address;
}

void InstructionBase::_set_address(addr_t address)
{
  _address = address & ARCH_BITMASK;
}

std::string InstructionBase::to_string() const
{
  // creating string streams: dynamically assembling strings
  std::ostringstream buffer;
  // gets the name and address of the current instruction
  const std::string instr_name = name();
  const int address = get_address();

  // generates a description string for the command according to the command name
  if (instr_name.compare(0, 3, "ADD") == 0)
    buffer << instr_name << ": ACC <- ACC + [" << address << "]";
  else if (instr_name.compare(0, 3, "AND") == 0)
    buffer << instr_name << ": ACC <- ACC & [" << address << "]";
  else if (instr_name.compare(0, 3, "ORR") == 0)
    buffer << instr_name << ": ACC <- ACC | [" << address << "]";
  else if (instr_name.compare(0, 3, "XOR") == 0)
    buffer << instr_name << ": ACC <- ACC ^ [" << address << "]";
  else if (instr_name.compare(0, 3, "LDR") == 0)
    buffer << instr_name << ": ACC <- [" << address << "]";
  else if (instr_name.compare(0, 3, "STR") == 0)
    buffer << instr_name << ": ACC -> [" << address << "]";
  else if (instr_name.compare(0, 3, "JMP") == 0)
    buffer << instr_name << ": PC  <- " << address;
  else if (instr_name.compare(0, 3, "JNE") == 0)
    buffer << instr_name << ": PC  <- " << address << " if ACC != 0";
  else
    assert(false);

  // returns assembled string
  return buffer.str();
}

// std::unique_ptr explicitly manages ownership of created objects
// prevent memory leaks and automatically manage object life cycle
std::unique_ptr<InstructionBase> InstructionBase::generateInstruction(InstructionData data)
{
  // process data.opcode by converting it to InstructionOpcode enumeration
  // change many if-else statements to switch-case for well arranged instructions
  switch (static_cast<InstructionOpcode>(data.opcode))
  {
  case ADD:
    return std::make_unique<Iadd>(addr_t{static_cast<int>(data.address)});
  case AND:
    return std::make_unique<Iand>(addr_t{static_cast<int>(data.address)});
  case ORR:
    return std::make_unique<Iorr>(addr_t{static_cast<int>(data.address)});
  case XOR:
    return std::make_unique<Ixor>(addr_t{static_cast<int>(data.address)});
  case LDR:
    return std::make_unique<Ildr>(addr_t{static_cast<int>(data.address)});
  case STR:
    return std::make_unique<Istr>(addr_t{static_cast<int>(data.address)});
  case JMP:
    return std::make_unique<Ijmp>(addr_t{static_cast<int>(data.address)});
  case JNE:
    return std::make_unique<Ijne>(addr_t{static_cast<int>(data.address)});
  default:
    return nullptr;
  }
}

// ========== ADD Instruction ==========
Iadd::Iadd(addr_t address)
{
  _set_address(address);
}

void Iadd::_execute(ProcessorState &state) const
{
  // using at to avoid error
  // same reason belows
  state.acc += state.memory.at(get_address());
}

// change char*(C language style) to std::string(C++ language style)
// match return type to header file
// same reason belows
std::string Iadd::name() const
{
  return "ADD";
}

// ========== AND Instruction ==========
Iand::Iand(addr_t address)
{
  _set_address(address);
}

void Iand::_execute(ProcessorState &state) const
{
  state.acc &= state.memory.at(get_address());
}

std::string Iand::name() const
{
  return "AND";
}

// ========== ORR Instruction ==========
Iorr::Iorr(addr_t address)
{
  _set_address(address);
}

void Iorr::_execute(ProcessorState &state) const
{
  state.acc |= state.memory.at(get_address());
}

std::string Iorr::name() const
{
  return "ORR";
}

// ========== XOR Instruction ==========
Ixor::Ixor(addr_t address)
{
  _set_address(address);
}

void Ixor::_execute(ProcessorState &state) const
{
  state.acc ^= state.memory.at(get_address());
}

std::string Ixor::name() const
{
  return "XOR";
}

// ========== LDR Instruction ==========
Ildr::Ildr(addr_t address)
{
  _set_address(address);
}

void Ildr::_execute(ProcessorState &state) const
{
  state.acc = state.memory.at(get_address());
}

std::string Ildr::name() const
{
  return "LDR";
}

// ========== STR Instruction ==========
Istr::Istr(addr_t address)
{
  _set_address(address);
}

void Istr::_execute(ProcessorState &state) const
{
  // type of memory array is byte_t
  // explicitly convert the state.acc value to byte_t to fit the memory size
  state.memory.at(get_address()) = static_cast<byte_t>(state.acc);
}

std::string Istr::name() const
{
  return "STR";
}

// ========== JMP Instruction ==========
Ijmp::Ijmp(addr_t address)
{
  _set_address(address);
}

void Ijmp::_execute(ProcessorState &state) const
{
  // Why minus two? Because execute() will increment PC by two,
  // so to make the PC take (eventually) the value `address`
  // I need to subtract two here. Same applies for JNE below
  // This kind of unintuitive behaviour is a clear sign of bad
  // class hierarchy design
  // INSTRUCTION_SIZE is equal to 2
  state.pc = get_address() - INSTRUCTION_SIZE;
}

std::string Ijmp::name() const
{
  return "JMP";
}

// ========== JNE Instruction ==========
Ijne::Ijne(addr_t address)
{
  _set_address(address);
}

void Ijne::_execute(ProcessorState &state) const
{
  // Same hack as above
  if (state.acc != 0)
    state.pc = get_address() - INSTRUCTION_SIZE;
}

std::string Ijne::name() const
{
  return "JNE";
}
