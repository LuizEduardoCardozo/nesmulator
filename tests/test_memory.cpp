#include "gtest/gtest.h"
#include "memory.h"

#define private public

TEST(MemoryTest, ShouldSetupWithAllMemoryAddressesEqualsToZero)
{
  auto memory = new Memory();
  uint32_t memory_size = memory->GetMemorySize();

  bool isDifferentThanZero = false;

  for (uint32_t i = 0; i < memory_size; i++)
    {
      isDifferentThanZero = (memory->GetMemory(i) != 0x0) && isDifferentThanZero;
    }

  EXPECT_FALSE(isDifferentThanZero);
}