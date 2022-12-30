#ifndef GOOGLETESTSEXAMPLE_CPU_H
#define GOOGLETESTSEXAMPLE_CPU_H

#include <vector>

#include "memory.h"
#include "utils/types.h"
#include "instruction.h"

class Cpu
{
  class Instruction
  {
  public:
    std::string name;
    byte (Cpu::*op)();
    byte (Cpu::*addr_mode)();
    byte cycles;

    Instruction();

    Instruction(std::string name, byte (Cpu::*op)(), byte (Cpu::*addr_mode)(), byte cycles)
    : name(std::move(name)), op(op), addr_mode(addr_mode), cycles(cycles) {}
  };
public:
  explicit Cpu(Memory* memory) : memory(memory) {}

  word PC = 0; // Program Counter
  byte SP = 0; // Stack Pointer

  byte A = 0, X = 0, Y = 0; // Registers

  byte cycles = 0;

  word status = 0x0;

  byte opcode = 0x0;
  byte fetched = 0x0;
  word addr_abs = 0x0;
  word addr_rel = 0x0;
  word temp = 0x0;

  word C = (1 << 0), // carry
    Z = (1 << 1), // zero
    I = (1 << 2), // disable interrupts
    D = (1 << 3), // decimal mode
    B = (1 << 4), // break
    U = (1 << 5), // unused
    V = (1 << 6), // overflow
    N = (1 << 7); // negative

  Memory* memory; // Internal memory

  bool GetFlag(word flag);
  void SetFlag(word flag, bool value);

  void Reset();
  void Irq();
  void Nmi(); // non-maskable interrupt

  void LDASetStatus();

  byte FetchByte();
  word FetchWord();

  byte ReadByte(byte add);

  byte fetch();

  void Execute(uint8_t cycles);

  // Addressing modes
  byte IMP();
  byte IMM();
  byte ZPX();
  byte ZPY();
  byte ZP0();
  byte REL();
  byte ABS();
  byte ABX();
  byte ABY();
  byte IND();
  byte IZX();
  byte IZY();

  // Instructions
  byte ADC();
  byte AND();
  byte ASL();
  byte BCC();
  byte BCS();
  byte BEQ();
  byte BIT();
  byte BMI();
  byte BNE();
  byte BPL();
  byte BRK();
  byte BVC();
  byte BVS();
  byte CLC();
  byte CLD();
  byte CLI();
  byte CLV();
  byte CMP();
  byte CPX();
  byte CPY();
  byte DEC();
  byte DEX();
  byte DEY();
  byte EOR();
  byte INC();
  byte INX();
  byte INY();
  byte JMP();
  byte JSR();
  byte LDA();
  byte LDX();
  byte LDY();
  byte LSR();
  byte NOP();
  byte ORA();
  byte PHA();
  byte PHP();
  byte PLA();
  byte PLP();
  byte ROL();
  byte ROR();
  byte RTI();
  byte RTS();
  byte SBC();
  byte SEC();
  byte SED();
  byte SEI();
  byte STA();
  byte STX();
  byte STY();
  byte TAX();
  byte TAY();
  byte TSX();
  byte TXA();
  byte TXS();
  byte TYA();
  byte XXX();

private:
   std::vector<Instruction> lookup;
};

#endif
