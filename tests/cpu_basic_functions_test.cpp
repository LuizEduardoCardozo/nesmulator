#include "gtest/gtest.h"

#include "cpu.h"
#include "memory.h"

struct TestStub
{
  Memory* memory;
  Cpu* cpu;
};

TestStub* setup_test_stub()
{
  auto *memory = new Memory();
  auto *cpu = new Cpu(memory);

  static TestStub stub = TestStub{
    memory,cpu
  };

  return &stub;
}

TEST(TestCpu, ResetShouldSetFlagC)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->SetFlag(stub->cpu->C, true);

  ASSERT_EQ(stub->cpu->status, 0b00000001);
}

TEST(TestCpu, ResetShouldSetFlagZ)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->SetFlag(stub->cpu->Z, true);

  std::cout << "status: " << stub->cpu->status << std::endl;

  ASSERT_EQ(stub->cpu->status, 0b00000010);
}

TEST(TestCpu, ResetShouldSetFlagI)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  stub->cpu->SetFlag(stub->cpu->I, true);

  ASSERT_EQ(stub->cpu->status, 0b00000100);
}

TEST(TestCpu, ResetShouldSetFlagD)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  stub->cpu->SetFlag(stub->cpu->D, true);

  ASSERT_EQ(stub->cpu->status, 0b00001000);
}

TEST(TestCpu, ResetShouldSetFlagB)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  stub->cpu->SetFlag(stub->cpu->B, true);

  ASSERT_EQ(stub->cpu->status, 0b00010000);
}

TEST(TestCpu, ResetShouldSetFlagU)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  stub->cpu->SetFlag(stub->cpu->U, true);

  ASSERT_EQ(stub->cpu->status, 0b00100000);
}

TEST(TestCpu, ResetShouldSetFlagV)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  stub->cpu->SetFlag(stub->cpu->V, true);

  ASSERT_EQ(stub->cpu->status, 0b01000000);
}

TEST(TestCpu, ResetShouldSetFlagN)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  stub->cpu->SetFlag(stub->cpu->N, true);

  ASSERT_EQ(stub->cpu->status, 0b10000000);
}

TEST(TestCpu, ResetShouldReadFlagCOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000001;

  bool flag = stub->cpu->GetFlag(
      stub->cpu->C
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldReadFlagCOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->C
  );

  ASSERT_FALSE(flag);
}

TEST(TestCpu, ResetShouldReadFlagZOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000010;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->Z
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldReadFlagZOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->Z
  );

  ASSERT_FALSE(flag);
}


TEST(TestCpu, ResetShouldReadFlagIOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000100;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->I
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldReadFlagIOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->I
  );

  ASSERT_FALSE(flag);
}

TEST(TestCpu, ResetShouldReadFlagDOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00001000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->D
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldReadFlagDOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->D
  );

  ASSERT_FALSE(flag);
}

TEST(TestCpu, ResetShouldReadFlagBOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00010000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->B
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldReadFlagBOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->B
  );

  ASSERT_FALSE(flag);
}

TEST(TestCpu, ResetShouldReadFlagUOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00100000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->U
  );
  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldReadFlagUOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->U
  );

  ASSERT_FALSE(flag);
}

TEST(TestCpu, ResetShouldSetFlagVOn)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b01000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->V
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldSetFlagVOff)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->V
  );

  ASSERT_FALSE(flag);
}

TEST(TestCpu, ResetShouldSetFlagNOn)
{
  TestStub* stub = setup_test_stub();

  stub->cpu->status = 0b10000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->N
  );

  ASSERT_TRUE(flag);
}

TEST(TestCpu, ResetShouldSetFlagNOff)
{
  TestStub* stub = setup_test_stub();

  stub->cpu->status = 0b00000000;

  bool flag = stub->cpu->GetFlag(
    stub->cpu->N
  );

  ASSERT_FALSE(flag);
}


TEST(TestCpu, ResetShouldSetRegisters)
{
  TestStub *stub = setup_test_stub();

  stub->cpu->Reset();

  ASSERT_EQ(stub->cpu->PC, 0x00);
  ASSERT_EQ(stub->cpu->SP, 0xFD);

  ASSERT_EQ(stub->cpu->status, 0x0);

  ASSERT_EQ(stub->cpu->A, 0x0);
  ASSERT_EQ(stub->cpu->X, 0x0);
  ASSERT_EQ(stub->cpu->Y, 0x0);
}

TEST(TestCpu, ShouldFetchDataFromMemory)
{
  TestStub *stub = setup_test_stub();

  byte data = 0x7F;

  stub->memory->SetMemory(data, stub->cpu->SP);

  byte fetched_data = stub->cpu->FetchByte();

  ASSERT_EQ(data, fetched_data);
}

TEST(TestCpu, ShouldFetchWordFromMemory)
{
  TestStub *stub = setup_test_stub();

  word data = 0xFFFE;

  stub->memory->WriteWord(data, stub->cpu->SP);

  word fetched_data = stub->cpu->FetchWord();

  ASSERT_EQ(data, fetched_data);
}

TEST(TestCpu, ShouldReadByteFromMemory)
{
  TestStub *stub = setup_test_stub();

  word data = 0xEE;
  byte addr = 0xA;

  stub->memory->SetMemory(data, addr);

  word read_data = stub->cpu->ReadByte(addr);

  ASSERT_EQ(data, read_data);
}
