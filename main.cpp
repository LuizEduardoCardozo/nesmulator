#include <iostream>

#include "cpu.h"
#include "memory.h"

int main()
{
  Memory *memory = new Memory();
  Cpu *cpu = new Cpu(memory);

  cpu->Reset();

  cpu->Execute(9);
}