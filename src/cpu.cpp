#include <iostream>

#include "cpu.h"

byte Cpu::fetch()
{
  if (lookup[opcode].addr_mode != &Cpu::IMP)
    fetched = memory->GetMemory(addr_abs);
  return fetched;
}

bool Cpu::GetFlag(word flag)
{
  return (status & flag) > 0;
}

void Cpu::SetFlag(word flag, bool value)
{
  status = value ? (status | flag) : (status & ~flag);
}

/*
 *  0xFFFC and 0xFFFD should contain 0x0 and 0x0.
 *  So, when the reset function reaches end, the program
 *  will be pointing to the start position of the memory
 */
void Cpu::Reset()
{
  word pc_lo = memory->GetMemory(0xFFFC);
  word pc_hi = memory->GetMemory(0xFFFD);

  PC = (pc_hi << 8) | pc_lo;
  SP = 0xFD; // startup value

  SetFlag(C, false);
  SetFlag(Z, false);
  SetFlag(I, false);
  SetFlag(D, false);
  SetFlag(B, false);
  // SetFlag(U, false);
  SetFlag(V, false);
  SetFlag(N, false);

  A = X = Y = 0;

  memory->Setup();

  cycles = 8; // the reset function consumes 8 cycles
}

// TODO: write tests
void Cpu::Irq()
{
  bool is_interrupt_allowed = !GetFlag(I);
  if (is_interrupt_allowed)
    {
      memory->SetMemory((PC >> 8) & 0x00FF, 0x0100 + SP);
      SP--;
      memory->SetMemory(PC & 0x00FF, 0x0100 + SP);
      SP--;

      SetFlag(B, false);
      SetFlag(U, true);
      SetFlag(I, true);

      memory->SetMemory(0x0100 + SP, status);
      SP--;

      word pc_lo = memory->GetMemory(0xFFFE);
      word pc_hi = memory->GetMemory(0xFFFF);

      PC = (pc_hi << 8) | pc_lo;

      cycles = 7;
    }
}

// TODO: write tests
void Cpu::Nmi()
{
  memory->SetMemory((PC >> 8) & 0x00FF, 0x0100 + SP);
  SP--;
  memory->SetMemory(PC & 0x00FF, 0x0100 + SP);
  SP--;

  SetFlag(B, false);
  SetFlag(U, true);
  SetFlag(I, true);

  memory->SetMemory(0x0100 + SP, status);
  SP--;

  word pc_lo = memory->GetMemory(0xFFFA);
  word pc_hi = memory->GetMemory(0xFFFB);

  PC = (pc_hi << 8) | pc_lo;

  cycles = 7;
}

byte Cpu::FetchByte()
{
  word program_counter_addr = PC;
  byte data = memory->GetMemory(program_counter_addr);

  PC++;
  cycles--;

  return data;
}

word Cpu::FetchWord()
{
  byte data_1 = memory->GetMemory(PC);
  PC++;
  byte data_2 = memory->GetMemory(PC);
  PC++;

  cycles -= 2;

  return (data_1 << 8) | data_2;
}

byte Cpu::ReadByte(byte addr)
{
  byte data = memory->GetMemory(addr);
  cycles--;
  return data;
}

void Cpu::LDASetStatus()
{
  Z = (A == 0);
  N = (A & 0b10000000) > 0;
}

void Cpu::Execute(byte _cycles)
{
  // std::cout << "Loaded" << lookupTable->table.size() << "instructions" << std::endl;
  this->cycles = _cycles;
  std::cout << "Calling execute, executing " << (int)this->cycles << " cycles" << std::endl;
  while (this->cycles > 0)
    {
      Instruction fetched_byte = FetchByte();
      auto instruction = static_cast<Instruction>(fetched_byte);

      switch (instruction)
        {
          case Instruction::INS_LDA_ZP: {
            std::cout << "INS_LDA_ZP" << std::endl;
            byte zero_page_addr = FetchByte();
            A = ReadByte(zero_page_addr);

            LDASetStatus();
          }
          break;
          case Instruction::INS_LDA_ZPX: {
            std::cout << "INS_LDA_ZPX" << std::endl;

            byte zero_page_addr = FetchByte();
            zero_page_addr += X;

            cycles--;

            A = ReadByte(zero_page_addr);

            LDASetStatus();
          }
          break;
          case Instruction::INS_LDA_IM: {
            std::cout << "INS_LDA_IM" << std::endl;

            byte val = FetchByte();
            A = val;

            LDASetStatus();
          }
          break;
          case Instruction::INS_JSR: {
            std::cout << "INS_JSR" << std::endl;

            word addr = FetchWord();
            memory->WriteWord(PC - 1, SP);

            PC = addr;

            cycles--;
          }
          break;
          default: {
            std::cout << "instruction not found " << instruction << std::endl;
          }
          break;
        }
    }
}

byte Cpu::IMP()
{
  fetched = A;
  return 0;
}

byte Cpu::IMM()
{
  addr_abs = PC++;
  return 0;
}

byte Cpu::ZP0()
{
  addr_abs = memory->GetMemory(PC);
  PC++;
  addr_abs &= 0x00FF;
  return 0;
}

byte Cpu::ZPX()
{
  addr_abs = memory->GetMemory(PC);
  addr_abs += X;
  addr_abs &= 0x00FF;
  return 0;
}

byte Cpu::ZPY()
{
  addr_abs = memory->GetMemory(PC);
  addr_abs += Y;
  addr_abs &= 0x00FF;
  return 0;
}

byte Cpu::REL()
{
  addr_rel = memory->GetMemory(PC);
  PC++;
  if (addr_rel & 0x80)
    addr_rel |= 0xFF00;
  return 0;
}

byte Cpu::ABS()
{
  word addr_lo = memory->GetMemory(PC);
  PC++;
  word addr_hi = memory->GetMemory(PC);
  PC++;
  addr_abs = (addr_hi << 8) | addr_lo;
  return 0;
}

byte Cpu::ABX()
{
  word addr_lo = memory->GetMemory(PC);
  PC++;
  word addr_hi = memory->GetMemory(PC);
  PC++;
  addr_abs = (addr_hi << 8) | addr_lo;
  addr_abs += X;
  return (addr_abs & 0xFF00) != (addr_hi << 8);
}

byte Cpu::ABY()
{
  word addr_lo = memory->GetMemory(PC);
  PC++;
  word addr_hi = memory->GetMemory(PC);
  PC++;
  addr_abs = (addr_hi << 8) | addr_lo;
  addr_abs += Y;
  return (addr_abs & 0xFF00) != (addr_hi << 8);
}

byte Cpu::IND()
{
  word ptr_lo = memory->GetMemory(PC);
  PC++;
  word ptr_hi = memory->GetMemory(PC);
  PC++;
  word ptr = (ptr_hi << 8) | ptr_lo;

  if (ptr_lo == 0x00FF)
    {
      addr_abs = (memory->GetMemory(ptr & 0xFF00) << 8) | memory->GetMemory(ptr);
    }
  else
    {
      addr_abs = (memory->GetMemory(ptr + 1) << 8) | memory->GetMemory(ptr);
    }
  return 0;
}

// Indirect X
byte Cpu::IZX()
{
  word val = memory->GetMemory(PC);
  PC++;

  word read_addr_lo = (word)(val + (word)X) & 0x00FF;
  word read_addr_hi = (word)(val + (word)X + 1) & 0x00FF;

  word addr_lo = memory->GetMemory(read_addr_lo);
  word addr_hi = memory->GetMemory(read_addr_hi);

  addr_abs = (addr_hi << 8) | addr_lo;

  return 0;
}

// Indirect Y
byte Cpu::IZY()
{
  word val = memory->GetMemory(PC);
  PC++;

  word read_addr_lo = val & 0x00FF;
  word read_addr_hi = (val + 1) & 0x00FF;

  word addr_lo = memory->GetMemory(read_addr_lo);
  word addr_hi = memory->GetMemory(read_addr_hi);

  addr_abs = (addr_hi << 8) | addr_lo;

  return (addr_abs & 0xFF00) != (addr_hi << 8);
};

byte Cpu::ADC()
{
  fetch();

  temp = (word)a + (word)fetched + (word)GetFlag(C);

  SetFlag(C, temp > 255);
  SetFlag(Z, (temp & 0x00FF) == 0);
  SetFlag(V, (~((word)a ^ (word)fetched) & ((word)a ^ (word)temp)) & 0x0080);
  SetFlag(N, temp & 0x80);

  A = temp & 0x00FF;

  return 1;
}


// Instruction: Subtraction with Borrow In
// Function:    A = A - M - (1 - C)
// Flags Out:   C, V, N, Z
//
// Explanation:
// Given the explanation for ADC above, we can reorganise our data
// to use the same computation for addition, for subtraction by multiplying
// the data by -1, i.e. make it negative
//
// A = A - M - (1 - C)  ->  A = A + -1 * (M - (1 - C))  ->  A = A + (-M + 1 + C)
//
// To make a signed positive number negative, we can invert the bits and add 1
// (OK, I lied, a little bit of 1 and 2s complement :P)
//
//  5 = 00000101
// -5 = 11111010 + 00000001 = 11111011 (or 251 in our 0 to 255 range)
//
// The range is actually unimportant, because if I take the value 15, and add 251
// to it, given we wrap around at 256, the result is 10, so it has effectively
// subtracted 5, which was the original intention. (15 + 251) % 256 = 10
//
// Note that the equation above used (1-C), but this got converted to + 1 + C.
// This means we already have the +1, so all we need to do is invert the bits
// of M, the data(!) therfore we can simply add, exactly the same way we did
// before.

byte Cpu::SBC()
{
  fetch();

  // Operating in 16-bit domain to capture carry out

  // We can invert the bottom 8 bits with bitwise xor
  word value = ((word)fetched) ^ 0x00FF;

  // Notice this is exactly the same as addition from here!
  temp = (word)a + value + (word)GetFlag(C);
  SetFlag(C, temp & 0xFF00);
  SetFlag(Z, ((temp & 0x00FF) == 0));
  SetFlag(V, (temp ^ (word)a) & (temp ^ value) & 0x0080);
  SetFlag(N, temp & 0x0080);
  a = temp & 0x00FF;
  return 1;
}

// OK! Complicated operations are done! the following are much simpler
// and conventional. The typical order of events is:
// 1) Fetch the data you are working with
// 2) Perform calculation
// 3) Store the result in desired place
// 4) Set Flags of the status register
// 5) Return if instruction has potential to require additional
//    clock cycle


// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
byte Cpu::AND()
{
  fetch();
  a = a & fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}


// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
byte Cpu::ASL()
{
  fetch();
  temp = (word)fetched << 1;
  SetFlag(C, (temp & 0xFF00) > 0);
  SetFlag(Z, (temp & 0x00FF) == 0x00);
  SetFlag(N, temp & 0x80);
  if (lookup[opcode].addrmode == &Cpu::IMP)
    a = temp & 0x00FF;
  else
    write(addr_abs, temp & 0x00FF);
  return 0;
}


// Instruction: Branch if Carry Clear
// Function:    if(C == 0) pc = address
byte Cpu::BCC()
{
  if (GetFlag(C) == 0)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}


// Instruction: Branch if Carry Set
// Function:    if(C == 1) pc = address
byte Cpu::BCS()
{
  if (GetFlag(C) == 1)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}


// Instruction: Branch if Equal
// Function:    if(Z == 1) pc = address
byte Cpu::BEQ()
{
  if (GetFlag(Z) == 1)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}

byte Cpu::BIT()
{
  fetch();
  temp = a & fetched;
  SetFlag(Z, (temp & 0x00FF) == 0x00);
  SetFlag(N, fetched & (1 << 7));
  SetFlag(V, fetched & (1 << 6));
  return 0;
}


// Instruction: Branch if Negative
// Function:    if(N == 1) pc = address
byte Cpu::BMI()
{
  if (GetFlag(N) == 1)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}


// Instruction: Branch if Not Equal
// Function:    if(Z == 0) pc = address
byte Cpu::BNE()
{
  if (GetFlag(Z) == 0)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}


// Instruction: Branch if Positive
// Function:    if(N == 0) pc = address
byte Cpu::BPL()
{
  if (GetFlag(N) == 0)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}

// Instruction: Break
// Function:    Program Sourced Interrupt
byte Cpu::BRK()
{
  pc++;

  SetFlag(I, 1);
  write(0x0100 + stkp, (pc >> 8) & 0x00FF);
  stkp--;
  write(0x0100 + stkp, pc & 0x00FF);
  stkp--;

  SetFlag(B, 1);
  write(0x0100 + stkp, status);
  stkp--;
  SetFlag(B, 0);

  pc = (word)read(0xFFFE) | ((word)read(0xFFFF) << 8);
  return 0;
}


// Instruction: Branch if Overflow Clear
// Function:    if(V == 0) pc = address
byte Cpu::BVC()
{
  if (GetFlag(V) == 0)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}


// Instruction: Branch if Overflow Set
// Function:    if(V == 1) pc = address
byte Cpu::BVS()
{
  if (GetFlag(V) == 1)
    {
      cycles++;
      addr_abs = pc + addr_rel;

      if ((addr_abs & 0xFF00) != (pc & 0xFF00))
        cycles++;

      pc = addr_abs;
    }
  return 0;
}


// Instruction: Clear Carry Flag
// Function:    C = 0
byte Cpu::CLC()
{
  SetFlag(C, false);
  return 0;
}


// Instruction: Clear Decimal Flag
// Function:    D = 0
byte Cpu::CLD()
{
  SetFlag(D, false);
  return 0;
}


// Instruction: Disable Interrupts / Clear Interrupt Flag
// Function:    I = 0
byte Cpu::CLI()
{
  SetFlag(I, false);
  return 0;
}


// Instruction: Clear Overflow Flag
// Function:    V = 0
byte Cpu::CLV()
{
  SetFlag(V, false);
  return 0;
}

// Instruction: Compare Accumulator
// Function:    C <- A >= M      Z <- (A - M) == 0
// Flags Out:   N, C, Z
byte Cpu::CMP()
{
  fetch();
  temp = (word)a - (word)fetched;
  SetFlag(C, a >= fetched);
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  return 1;
}


// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
byte Cpu::CPX()
{
  fetch();
  temp = (word)x - (word)fetched;
  SetFlag(C, x >= fetched);
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  return 0;
}


// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
byte Cpu::CPY()
{
  fetch();
  temp = (word)y - (word)fetched;
  SetFlag(C, y >= fetched);
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  return 0;
}


// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
byte Cpu::DEC()
{
  fetch();
  temp = fetched - 1;
  write(addr_abs, temp & 0x00FF);
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  return 0;
}


// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
byte Cpu::DEX()
{
  x--;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}


// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
byte Cpu::DEY()
{
  y--;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 0;
}


// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
byte Cpu::EOR()
{
  fetch();
  a = a ^ fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}


// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
byte Cpu::INC()
{
  fetch();
  temp = fetched + 1;
  write(addr_abs, temp & 0x00FF);
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  return 0;
}


// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
byte Cpu::INX()
{
  x++;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}


// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
byte Cpu::INY()
{
  y++;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 0;
}


// Instruction: Jump To Location
// Function:    pc = address
byte Cpu::JMP()
{
  pc = addr_abs;
  return 0;
}


// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, pc = address
byte Cpu::JSR()
{
  pc--;

  write(0x0100 + stkp, (pc >> 8) & 0x00FF);
  stkp--;
  write(0x0100 + stkp, pc & 0x00FF);
  stkp--;

  pc = addr_abs;
  return 0;
}


// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
byte Cpu::LDA()
{
  fetch();
  a = fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}


// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
byte Cpu::LDX()
{
  fetch();
  x = fetched;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 1;
}


// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
byte Cpu::LDY()
{
  fetch();
  y = fetched;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 1;
}

byte Cpu::LSR()
{
  fetch();
  SetFlag(C, fetched & 0x0001);
  temp = fetched >> 1;
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  if (lookup[opcode].addrmode == &Cpu::IMP)
    a = temp & 0x00FF;
  else
    write(addr_abs, temp & 0x00FF);
  return 0;
}

byte Cpu::NOP()
{
  // Sadly not all NOPs are equal, Ive added a few here
  // based on https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
  // and will add more based on game compatibility, and ultimately
  // I'd like to cover all illegal opcodes too
  switch (opcode) {
    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
      return 1;
      break;
    }
  return 0;
}


// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
byte Cpu::ORA()
{
  fetch();
  a = a | fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}


// Instruction: Push Accumulator to Stack
// Function:    A -> stack
byte Cpu::PHA()
{
  write(0x0100 + stkp, a);
  stkp--;
  return 0;
}


// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
byte Cpu::PHP()
{
  write(0x0100 + stkp, status | B | U);
  SetFlag(B, 0);
  SetFlag(U, 0);
  stkp--;
  return 0;
}


// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
byte Cpu::PLA()
{
  stkp++;
  a = read(0x0100 + stkp);
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 0;
}


// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
byte Cpu::PLP()
{
  stkp++;
  status = read(0x0100 + stkp);
  SetFlag(U, 1);
  return 0;
}

byte Cpu::ROL()
{
  fetch();
  temp = (word)(fetched << 1) | GetFlag(C);
  SetFlag(C, temp & 0xFF00);
  SetFlag(Z, (temp & 0x00FF) == 0x0000);
  SetFlag(N, temp & 0x0080);
  if (lookup[opcode].addrmode == &Cpu::IMP)
    a = temp & 0x00FF;
  else
    write(addr_abs, temp & 0x00FF);
  return 0;
}

byte Cpu::ROR()
{
  fetch();
  temp = (word)(GetFlag(C) << 7) | (fetched >> 1);
  SetFlag(C, fetched & 0x01);
  SetFlag(Z, (temp & 0x00FF) == 0x00);
  SetFlag(N, temp & 0x0080);
  if (lookup[opcode].addrmode == &Cpu::IMP)
    a = temp & 0x00FF;
  else
    write(addr_abs, temp & 0x00FF);
  return 0;
}

byte Cpu::RTI()
{
  stkp++;
  status = read(0x0100 + stkp);
  status &= ~B;
  status &= ~U;

  stkp++;
  pc = (word)read(0x0100 + stkp);
  stkp++;
  pc |= (word)read(0x0100 + stkp) << 8;
  return 0;
}

byte Cpu::RTS()
{
  stkp++;
  pc = (word)read(0x0100 + stkp);
  stkp++;
  pc |= (word)read(0x0100 + stkp) << 8;

  pc++;
  return 0;
}




// Instruction: Set Carry Flag
// Function:    C = 1
byte Cpu::SEC()
{
  SetFlag(C, true);
  return 0;
}


// Instruction: Set Decimal Flag
// Function:    D = 1
byte Cpu::SED()
{
  SetFlag(D, true);
  return 0;
}


// Instruction: Set Interrupt Flag / Enable Interrupts
// Function:    I = 1
byte Cpu::SEI()
{
  SetFlag(I, true);
  return 0;
}


// Instruction: Store Accumulator at Address
// Function:    M = A
byte Cpu::STA()
{
  write(addr_abs, a);
  return 0;
}


// Instruction: Store X Register at Address
// Function:    M = X
byte Cpu::STX()
{
  write(addr_abs, x);
  return 0;
}


// Instruction: Store Y Register at Address
// Function:    M = Y
byte Cpu::STY()
{
  write(addr_abs, y);
  return 0;
}


// Instruction: Transfer Accumulator to X Register
// Function:    X = A
// Flags Out:   N, Z
byte Cpu::TAX()
{
  x = a;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}


// Instruction: Transfer Accumulator to Y Register
// Function:    Y = A
// Flags Out:   N, Z
byte Cpu::TAY()
{
  y = a;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 0;
}


// Instruction: Transfer Stack Pointer to X Register
// Function:    X = stack pointer
// Flags Out:   N, Z
byte Cpu::TSX()
{
  x = stkp;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}


// Instruction: Transfer X Register to Accumulator
// Function:    A = X
// Flags Out:   N, Z
byte Cpu::TXA()
{
  a = x;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 0;
}


// Instruction: Transfer X Register to Stack Pointer
// Function:    stack pointer = X
byte Cpu::TXS()
{
  stkp = x;
  return 0;
}


// Instruction: Transfer Y Register to Accumulator
// Function:    A = Y
// Flags Out:   N, Z
byte Cpu::TYA()
{
  a = y;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 0;
}


// This function captures illegal opcodes
byte Cpu::XXX()
{
  return 0;
}






/*
// This is the disassembly function. Its workings are not required for emulation.
// It is merely a convenience function to turn the binary instruction code into
// human readable form. Its included as part of the emulator because it can take
// advantage of many of the CPUs internal operations to do this.
std::map<word, std::string> Cpu::disassemble(word nStart, word nStop)
{
  uint32_t addr = nStart;
  byte value = 0x00, lo = 0x00, hi = 0x00;
  std::map<word, std::string> mapLines;
  word line_addr = 0;

  // A convenient utility to convert variables into
  // hex strings because "modern C++"'s method with
  // streams is atrocious
  auto hex = [](uint32_t n, byte d)
  {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  // Starting at the specified address we read an instruction
  // byte, which in turn yields information from the lookup table
  // as to how many additional bytes we need to read and what the
  // addressing mode is. I need this info to assemble human readable
  // syntax, which is different depending upon the addressing mode

  // As the instruction is decoded, a std::string is assembled
  // with the readable output
  while (addr <= (uint32_t)nStop)
    {
      line_addr = addr;

      // Prefix line with instruction address
      std::string sInst = "$" + hex(addr, 4) + ": ";

      // Read instruction, and get its readable name
      byte opcode = bus->read(addr, true); addr++;
      sInst += lookup[opcode].name + " ";

      // Get oprands from desired locations, and form the
      // instruction based upon its addressing mode. These
      // routines mimmick the actual fetch routine of the
      // 6502 in order to get accurate data as part of the
      // instruction
      if (lookup[opcode].addrmode == &Cpu::IMP)
        {
          sInst += " {IMP}";
        }
      else if (lookup[opcode].addrmode == &Cpu::IMM)
        {
          value = bus->read(addr, true); addr++;
          sInst += "#$" + hex(value, 2) + " {IMM}";
        }
      else if (lookup[opcode].addrmode == &Cpu::ZP0)
        {
          lo = bus->read(addr, true); addr++;
          hi = 0x00;
          sInst += "$" + hex(lo, 2) + " {ZP0}";
        }
      else if (lookup[opcode].addrmode == &Cpu::ZPX)
        {
          lo = bus->read(addr, true); addr++;
          hi = 0x00;
          sInst += "$" + hex(lo, 2) + ", X {ZPX}";
        }
      else if (lookup[opcode].addrmode == &Cpu::ZPY)
        {
          lo = bus->read(addr, true); addr++;
          hi = 0x00;
          sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
        }
      else if (lookup[opcode].addrmode == &Cpu::IZX)
        {
          lo = bus->read(addr, true); addr++;
          hi = 0x00;
          sInst += "($" + hex(lo, 2) + ", X) {IZX}";
        }
      else if (lookup[opcode].addrmode == &Cpu::IZY)
        {
          lo = bus->read(addr, true); addr++;
          hi = 0x00;
          sInst += "($" + hex(lo, 2) + "), Y {IZY}";
        }
      else if (lookup[opcode].addrmode == &Cpu::ABS)
        {
          lo = bus->read(addr, true); addr++;
          hi = bus->read(addr, true); addr++;
          sInst += "$" + hex((word)(hi << 8) | lo, 4) + " {ABS}";
        }
      else if (lookup[opcode].addrmode == &Cpu::ABX)
        {
          lo = bus->read(addr, true); addr++;
          hi = bus->read(addr, true); addr++;
          sInst += "$" + hex((word)(hi << 8) | lo, 4) + ", X {ABX}";
        }
      else if (lookup[opcode].addrmode == &Cpu::ABY)
        {
          lo = bus->read(addr, true); addr++;
          hi = bus->read(addr, true); addr++;
          sInst += "$" + hex((word)(hi << 8) | lo, 4) + ", Y {ABY}";
        }
      else if (lookup[opcode].addrmode == &Cpu::IND)
        {
          lo = bus->read(addr, true); addr++;
          hi = bus->read(addr, true); addr++;
          sInst += "($" + hex((word)(hi << 8) | lo, 4) + ") {IND}";
        }
      else if (lookup[opcode].addrmode == &Cpu::REL)
        {
          value = bus->read(addr, true); addr++;
          sInst += "$" + hex(value, 2) + " [$" + hex(addr + value, 4) + "] {REL}";
        }

      // Add the formed string to a std::map, using the instruction's
      // address as the key. This makes it convenient to look for later
      // as the instructions are variable in length, so a straight up
      // incremental index is not sufficient.
      mapLines[line_addr] = sInst;
    }

  return mapLines;
}
*/
