#include <algorithm>

#include "memory.h"

void Memory::Setup()
{
  byte initial_memory_value = 0x0;
  std::fill(
    std::begin(memory),
    std::end(memory),
    initial_memory_value
    );
}

byte Memory::GetMemory(word addr) const
{
  return memory[addr];
}

void Memory::SetMemory(byte data, word addr)
{
  memory[addr] = data;
}

void Memory::WriteWord(word value, uint16_t addr)
{
  byte p1 = value >> 8;
  byte p2 = (value << 8) >> 8;

  memory[addr] = p1;
  memory[addr+1] = p2;
}

uint32_t Memory::GetMemorySize()
{
  return MEM_SIZE;
}