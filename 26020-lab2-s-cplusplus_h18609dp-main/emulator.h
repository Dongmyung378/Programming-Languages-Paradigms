#pragma once

// modified by Dongmyung

// basic libraries are used in this file
#include "common.h"
#include <string>
#include <vector>
#include <memory>

//------------------------------------------------------------------------------
//--------------------               CONSTANTS              --------------------
//------------------------------------------------------------------------------

// using constexpr for type safety and scoped constants
constexpr int MAX_INSTRUCTIONS = ((MEMORY_SIZE) / (INSTRUCTION_SIZE));

//------------------------------------------------------------------------------
//--------------------            CLASS DECLARATIONS        --------------------
//------------------------------------------------------------------------------

/**
 * A representation of a breakpoint.
 * This includes the actual address we break on and a symbolic name that
 * the application frontend and the user might use to refer to the breakpoint
 */
class Breakpoint
{
public:
  // virtual destructor makes the object will be destroyed correctly. this will prevent memory leakage or resource release problems
  virtual ~Breakpoint() = default;

  // Default Constructor
  // remove char* and use std::string for ensure the RAII
  Breakpoint(addr_t address, const std::string &name);

  // Copy/Move Constructors
  Breakpoint(const Breakpoint &other);
  Breakpoint(Breakpoint &&other) noexcept;
  // Copy/Move Assignment Operators
  Breakpoint &operator=(const Breakpoint &other);
  Breakpoint &operator=(Breakpoint &&other) noexcept;

  // Getter for the address
  addr_t get_address() const;

  // Getter for the name
  // use std::string instead of char*
  const std::string &get_name() const;

  // Change return type to boolean type to intuitively represent results
  // Testing whether the breakpoint targets this address
  bool has(addr_t address) const;

  // Change return type to boolean type to intuitively represent results
  // Testing whether the breakpoint targets this name
  bool has(const std::string &name) const;

private:
  addr_t _address;
  // comply with RAII for code safety and else
  std::string _name;
};

class Emulator
{
public:
  // virtual destructor makes the object will be destroyed correctly. this will prevent memory leakage or resource release problems
  virtual ~Emulator() = default;

  // Default constructor
  Emulator();

  // Copy/Move Constructors
  Emulator(const Emulator &other);
  Emulator(Emulator &&other) noexcept;
  // Copy/Move Assignment Operators
  Emulator &operator=(const Emulator &other);
  Emulator &operator=(Emulator &&other) noexcept;

  // Get the two bytes representing the next instruction to execute
  InstructionData fetch() const;

  // returns a smart pointer that takes ownership of the object
  // smart pointers automatically manage an object's life cycle, so there is no risk of memory leakage
  std::unique_ptr<InstructionBase> decode(InstructionData instruction) const;

  // A simple function just calling the instructions execute function
  // Change return type to boolean type to intuitively represent results
  bool execute(const InstructionBase *instr);

  // Change return type to boolean type to intuitively represent results
  // Run iterations for a certain number of steps, until an error happens, or we reach a breakpoint
  bool run(int steps);

  // Change return type to boolean type to intuitively represent results
  // Register a new breakpoint with the given address and name
  bool insert_breakpoint(addr_t address, const std::string &name);

  // std::shared_ptr manage the life cycle of an object based on the reference count
  // std::shared_ptr allows multiple shared_ptr instances to share ownership of objects
  // the last shared_ptr destruction automatically releases the object's memory
  std::shared_ptr<const Breakpoint> find_breakpoint(addr_t address) const;
  std::shared_ptr<const Breakpoint> find_breakpoint(const std::string &name) const;

  // Change return type to boolean type to intuitively represent results
  // Unregister the breakpoint with the given address
  bool delete_breakpoint(addr_t address);

  // Change return type to boolean type to intuitively represent results
  // Unregister the breakpoint with the given name
  // change char* to std::string type
  bool delete_breakpoint(const std::string &name);

  // Get the number of registered breakpoints
  int num_breakpoints() const;

  // Return the total number of cycles executes so far
  int cycles() const;

  // Getter for the accumulator
  data_t read_acc() const;

  // Getter for the PC
  addr_t read_pc() const;

  // Read a memory byte
  addr_t read_mem(addr_t address) const;

  // Check whether the accumulator is zero
  // Change return type to boolean type to intuitively represent results
  bool is_zero() const;

  // Do we have a breakpoint for the current PC
  // Change return type to boolean type to intuitively represent results
  bool is_breakpoint() const;

  // Prints on stdout all the state of the program
  // Change return type to boolean type to intuitively represent results
  bool print_program() const;

  /**
   * Reads the processor state from a file
   * The format is:
   * line 1 -> total number of cycles executed so far
   * line 2 -> value of acc
   * line 3 -> value of pc
   * line 4-259 -> All 256 memory bytes in their memory order. Each byte is printed as an unsigned number in its own line.
   * line 260-end -> One line for each active breakpoint, each line containing the address and name of the breakpoint separated by one space
   */
  // Change return type to boolean type to intuitively represent results
  bool load_state(const std::string &state_filename);

  /**
   * Stores the processor state in a file, in the same format used by load_state
   *
   * Same format as above
   */
  // Change return type to boolean type to intuitively represent results
  // using std::string for RAII
  bool save_state(const std::string &state_filename) const;

private:
  ProcessorState state;
  /* std::vector automatically handles dynamic array sizing and memory management.
   * when an array grows or shrinks in size, it internally reallocates memory and performs the necessary actions.
   */
  std::vector<std::shared_ptr<Breakpoint>> breakpoints{};
  int total_cycles;
};