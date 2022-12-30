#ifndef GOOGLETESTSEXAMPLE_MEMORY_H
#define GOOGLETESTSEXAMPLE_MEMORY_H

#include "utils/types.h"

class Memory {
public:
    void Setup();

    byte GetMemory(word addr) const;
    void SetMemory(byte data, word addr);

    void WriteWord(word value, uint16_t addr);

    static uint32_t GetMemorySize();

  private:
    static constexpr uint32_t MEM_SIZE = 1024 * 64;
    byte memory[MEM_SIZE];
};

#endif
