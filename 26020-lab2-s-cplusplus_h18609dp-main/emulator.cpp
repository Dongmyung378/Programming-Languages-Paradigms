// modified by Dongmyung

// basic libraries are used in this file
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include "emulator.h"

// ============= Breakpoint ==============
// initializes the breakpoint object
Breakpoint::Breakpoint(addr_t address, const std::string &name)
    // restrict address to 8-bit range, initialize to forwarded name
    : _address{addr_t{address & ARCH_BITMASK}}, _name{name}
{
}

// Copy constructor
// initialize the new Breakpoint object by copying the member values of other Breakpoint objects as they are
Breakpoint::Breakpoint(const Breakpoint &other)
    : _address{other._address}, _name{other._name}
{
}

// Move constructor
// move the memory of an existing object to a new object through std::move
Breakpoint::Breakpoint(Breakpoint &&other) noexcept
    : _address{std::move(other._address)}, _name{std::move(other._name)}
{
}

// Copy assignment
// copy the value of another Breakpoint object to an existing Breakpoint object
Breakpoint &Breakpoint::operator=(const Breakpoint &other)
{
  // prevent substitution with myself
  if (this == &other)
    return *this;
  _address = other._address;
  _name = other._name;
  // return myself after completing copy
  return *this;
}

// Move assignment
// move resources from another Breakpoint object to an existing Breakpoint object
Breakpoint &Breakpoint::operator=(Breakpoint &&other) noexcept
{
  // prevent substitution with myself
  if (this == &other)
    return *this;
  _address = std::move(other._address);
  _name = std::move(other._name);
  // return myself after completing copy
  return *this;
}

// returns the address of the breakpoint
addr_t Breakpoint::get_address() const
{
  return _address;
}

// returns the name of the breakpoint
const std::string &Breakpoint::get_name() const
{
  return _name;
}

// checks if the given address matches the address of this breakpoint
bool Breakpoint::has(addr_t address) const
{
  return _address == (address & ARCH_BITMASK);
}

// checks if the given name matches the name of this breakpoint
bool Breakpoint::has(const std::string &name) const
{
  return _name == name;
}

// default constructor for the Emulator class
Emulator::Emulator()
    // reset processor state, initialize breakpoint list, initialize run cycle
    : state{}, breakpoints{}, total_cycles{0}
{
}

// Copy Constructor
// creates a new object by copying the state of the given Emulator object
Emulator::Emulator(const Emulator &other)
    // copy processor state, copy execution cycle
    : state{other.state}, total_cycles{other.total_cycles}
{
  // breakpoint memory reservation
  breakpoints.reserve(other.breakpoints.size());
  // copy breakpoints
  for (const auto &bp : other.breakpoints)
  {
    // create a new shared_ptr
    breakpoints.push_back(std::make_shared<Breakpoint>(*bp));
  }
}

// Move Constructor
// moves the resources of a given Emulator object to a new object
Emulator::Emulator(Emulator &&other) noexcept
    // move processor state, move breakpoint list, copy total cycles
    : state{std::move(other.state)}, breakpoints{std::move(other.breakpoints)}, total_cycles{other.total_cycles}
{
}

// Copy Assignment Operator
// copies the values ​​of the given Emulator object to an existing object
Emulator &Emulator::operator=(const Emulator &other)
{
  // prevent substitution with myself
  if (this == &other)
    return *this;
  // copy processor state
  state = other.state;
  // copy total cycles
  total_cycles = other.total_cycles;

  // remove breakpoint
  breakpoints.clear();
  // breakpoint memory reservation
  breakpoints.reserve(other.breakpoints.size());
  // copy new breakpoints
  for (const auto &bp : other.breakpoints)
  {
    // create a new shared_ptr
    breakpoints.push_back(std::make_shared<Breakpoint>(*bp));
  }
  return *this;
}

// Move Assignment Operator
// moves the resources of a given Emulator object to an existing object
Emulator &Emulator::operator=(Emulator &&other) noexcept
{
  if (this == &other)
    return *this;
  // move processor state
  state = std::move(other.state);
  // move breakpoint list, copy total cycles
  breakpoints = std::move(other.breakpoints);
  total_cycles = other.total_cycles;

  return *this;
}

InstructionData Emulator::fetch() const
{
  // set initial value as 0, 0
  InstructionData data = {0, 0};
  // using at to avoid error
  data.opcode = state.memory.at(state.pc);
  data.address = state.memory.at(state.pc + 1);
  return data;
}

// using std::unique_ptr
// match return type to header file
std::unique_ptr<InstructionBase> Emulator::decode(InstructionData data) const
{
  // decode here is just a thin wrapper around generateInstruction()
  // In a more complex emulator, more things would happen here
  return InstructionBase::generateInstruction(data);
}

bool Emulator::execute(const InstructionBase *instr)
{
  // Again this is just a thin wrapper,
  // but this is a side-effect of having a simple emulator
  instr->execute(state);
  return true;
}

// at each step, instructions are fetched, decoded, and executed
// returns false if the Program Counter (PC) is invalid or an error occurs during execution
// when a breakpoint is reached, it stops execution and returns true
bool Emulator::run(int steps)
{
  if (steps == 0)
    return true;

  while (steps-- > 0)
  {
    // if the number of PCs is odd, it is in an invalid state
    if ((state.pc % 2) == 1)
      return false;

    // fetch and decode instruction
    // auto allows the compiler to automatically determine the type of a variable through type inference
    auto instr_data = fetch();
    auto instr = decode(instr_data);
    if (!instr)
      return false;

    // execute instruction
    if (!execute(instr.get()))
      return false;

    // total_cycles + 1
    ++total_cycles;

    if (is_breakpoint())
      return true;
  }
  return true;
}

bool Emulator::insert_breakpoint(addr_t address, const std::string &name)
{

  // Breakpoint already exists
  if (find_breakpoint(address) != nullptr || find_breakpoint(name) != nullptr)
    return false;

  // create and register a new breakpoint
  // memory management with shared pointers
  auto bp = std::make_shared<Breakpoint>(address, name);
  // add to breakpoint list
  breakpoints.emplace_back(bp);
  return true;
}

// find the breakpoint matching the given address in the breakpoint list
std::shared_ptr<const Breakpoint> Emulator::find_breakpoint(addr_t address) const
{
  for (const auto &bp : breakpoints)
  {
    // address matches
    if (bp->has(address))
    {
      // return the breakpoint
      return bp;
    }
  }
  // return nullptr
  return nullptr;
}

// find the breakpoint matching the given name in the breakpoint list
std::shared_ptr<const Breakpoint> Emulator::find_breakpoint(const std::string &name) const
{
  for (const auto &bp : breakpoints)
  {
    if (bp->has(name))
    {
      return bp;
    }
  }
  return nullptr;
}

// finds and removes breakpoints matching a given address in the breakpoint list
bool Emulator::delete_breakpoint(addr_t address)
{
  // find breakpoints with matching addresses
  auto it = std::find_if(breakpoints.begin(), breakpoints.end(),
                         [address](const std::shared_ptr<Breakpoint> &bp)
                         { return bp->has(address); });
  // delete breakpoint if it exists
  if (it != breakpoints.end())
  {
    // remove that breakpoint from the vector
    breakpoints.erase(it);
    return true;
  }
  return false;
}

// finds and removes breakpoints matching a given name in the breakpoint list
bool Emulator::delete_breakpoint(const std::string &name)
{
  auto it = std::find_if(breakpoints.begin(), breakpoints.end(),
                         [&name](const std::shared_ptr<Breakpoint> &bp)
                         { return bp->has(name); });

  if (it != breakpoints.end())
  {
    breakpoints.erase(it);
    return true;
  }
  return false;
}

// returns the number of currently registered breakpoints
int Emulator::num_breakpoints() const
{
  // returns the size of the vector as an integer
  return static_cast<int>(breakpoints.size());
}

// returns the total number of cycles executed by the emulator
int Emulator::cycles() const
{
  return total_cycles;
}

data_t Emulator::read_acc() const
{
  return state.acc;
}

addr_t Emulator::read_pc() const
{
  return state.pc;
}

addr_t Emulator::read_mem(addr_t address) const
{
  // limit address to the allowed range of values
  address &= ARCH_BITMASK;
  // reads the memory value from the specified address and returns it as an integer type
  return addr_t{static_cast<int>(state.memory.at(address))};
}

bool Emulator::is_zero() const
{
  return state.acc == 0;
}

bool Emulator::is_breakpoint() const
{
  // change NULL to nullptr
  return find_breakpoint(state.pc) != nullptr;
}

// outputs program instructions stored in memory
bool Emulator::print_program() const
{
  for (int offset = 0; offset < MEMORY_SIZE; offset += INSTRUCTION_SIZE)
  {
    InstructionData data = {0, 0};
    data.opcode = state.memory.at(offset);
    data.address = state.memory.at(offset + 1);
    // instruction decode
    auto instr = InstructionBase::generateInstruction(data);

    // command information output
    // if the command is empty or invalid
    if (!instr || (data.opcode == 0 && data.address == 0))
      std::cout << offset << ":\t" << static_cast<int>(data.opcode) << "\t" << static_cast<int>(data.address) << "\n";
    // in case of valid commands, decoded information is also output
    else
      std::cout << offset << ":\t" << static_cast<int>(data.opcode) << "\t" << static_cast<int>(data.address)
                << "\t:\t" << instr->to_string() << "\n";
  }
  return true;
}

// load emulator state from file
bool Emulator::load_state(const std::string &filename)
{
  // Delete all breakpoints
  breakpoints.clear();

  // use std::ifstream to open file
  // std::ifstream is C++ style and automatically manages resource
  std::ifstream infile(filename);
  if (!infile)
    return false;

  if (!(infile >> total_cycles) || total_cycles < 0)
    return false;

  if (!(infile >> state.acc) || state.acc > ARCH_MAXVAL || state.acc < 0)
    return false;

  if (!(infile >> state.pc) || state.pc >= MEMORY_SIZE || state.pc < 0)
    return false;

  // load memory value
  int num = 0;

  for (int offset = 0; offset < MEMORY_SIZE; ++offset)
  {
    if (!(infile >> num) || num > ARCH_MAXVAL || num < 0)
      return false;
    state.memory.at(offset) = static_cast<byte_t>(num);
  }

  // load breakpoint information
  int addr = 0;
  std::string name;

  while (infile >> addr >> name)
  {
    if (addr < 0 || addr >= MEMORY_SIZE)
      return false;
    if (!insert_breakpoint(addr_t{addr}, name))
      return false;
  }

  return true;
}

// save emulator state to file
bool Emulator::save_state(const std::string &filename) const
{
  // use std::ofstream to open file
  // std::ofstream is C++ style and automatically manages resource
  std::ofstream outfile(filename);
  if (!outfile)
    return false;

  outfile << total_cycles << "\n";
  outfile << state.acc << "\n";
  outfile << state.pc << "\n";

  // store memory value
  for (int offset = 0; offset < MEMORY_SIZE; ++offset)
  {
    outfile << static_cast<int>(state.memory.at(offset)) << "\n";
  }

  // save breakpoint information
  for (const auto &bp : breakpoints)
  {
    outfile << bp->get_address() << " " << bp->get_name() << "\n";
  }

  return true;
}
