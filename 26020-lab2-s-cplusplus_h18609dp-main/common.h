#pragma once

// modified by Dongmyung

// basic libraries are used in this file
#include <string>
#include <array>
#include <memory>
#include <cstdint>

//------------------------------------------------------------------------------
//--------------------               CONSTANTS              --------------------
//------------------------------------------------------------------------------

// using constexpr for type safety and scoped constants
constexpr int ARCH_BITS = 8;
constexpr int ARCH_BITMASK = (1 << ARCH_BITS) - 1;
constexpr int ARCH_MAXVAL = ARCH_BITMASK;
constexpr int INSTRUCTION_SIZE = 2;
constexpr int MEMORY_SIZE = 256;
constexpr int MAX_NAME = 96;

//------------------------------------------------------------------------------
//--------------------             HELPER TYPES             --------------------
//------------------------------------------------------------------------------

// using using makes the code easy to read and modern type
// Use the type `addr_t` for variables holding generic data
using addr_t = int;
// Use the type `data_t` for variables holding generic data
using data_t = int;

// Use the type byte_t for memory locations.
// Here, for storage efficiency, we actually use only 8 bits for each byte.
// Btw, uint8_t is an integer type guaranteed to be unsigned and 8 bits wide.
using byte_t = uint8_t;

struct InstructionData
{
  byte_t opcode;  // operation code identifying the command type
  byte_t address; // memory location related to instruction
};

struct ProcessorState
{
  data_t acc = 0; // set value to 0
  addr_t pc = 0;  // set value to 0

  // memory array, replaced with std::array<type, size>, set all elements in memory to 0
  std::array<byte_t, MEMORY_SIZE> memory{};
};

//------------------------------------------------------------------------------
//--------------------            CLASS DECLARATIONS        --------------------
//------------------------------------------------------------------------------

class InstructionBase
{
public:
  // virtual destructor makes the object will be destroyed correctly. this will prevent memory leakage or resource release problems
  virtual ~InstructionBase() = default;

  // copy constructor, copy assignment operator, move constructor, move assignment operator
  // this 4 lines for avoiding warning from clag-tidy actually no use
  // default: The compiler automatically generates the appropriate copy generator and copy substitution operator
  // neexcept: no thorwing exception which improves performance
  InstructionBase(const InstructionBase &other) = default;
  InstructionBase &operator=(const InstructionBase &other) = default;
  InstructionBase(InstructionBase &&other) noexcept = default;
  InstructionBase &operator=(InstructionBase &&other) noexcept = default;

  void execute(ProcessorState &state) const;

  // Convenience getter for _address
  addr_t get_address() const;

  // change char* to std::string for making code be modern
  // memory is automatically released when the object is destroyed, so there is no risk of memory leakage(following RAII)
  // std::string is return type, to_string is function, and const represent function do not change the state of object
  std::string to_string() const;

  /**
   * The instruction-specific functionality of executing an instruction.
   *
   * Declared unimplemented and virtual here.
   * Subclasses provide the concrete, instruction-specific behaviour.
   * @param state the processor state we operate on
   */
  virtual void _execute(ProcessorState &state) const = 0;

  // this line is also change char* to std::string for same reason
  // returns the mnemonic name of the instruction
  virtual std::string name() const = 0;

  /* std::unique_ptr is an explicit representation of ownership,
  automatically managing the object's life cycle. This prevents the hassle
  of manually calling delete and memory leaks(following RAII) */
  static std::unique_ptr<InstructionBase> generateInstruction(InstructionData data);

protected:
  // initializes the _address member variable to 0
  InstructionBase() : _address(0) {};

  /**
   * A convenience protected setter for _address
   *
   * This is needed only for initialisation in the derived classes constructors,
   * so we need at least `protected` visibility.
   * _address is not meant to change after initialisation, so this setter
   * should not be public.
   * Since this is protected you can remove it, if your code does not need it.
   *
   * @param address The address to set
   */
  void _set_address(addr_t address);

private:
  addr_t _address;
};
