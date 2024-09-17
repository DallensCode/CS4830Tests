#include <gtest/gtest.h>
#include <cstring>
#include <iostream>

#include "emu4380.h"

TEST(initMemTests, testInitMem_noParams) {
  init_mem();
  EXPECT_EQ(131072, PROG_MEM_SIZE);
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(initMemTests, testInitMem_userParams) {
  init_mem(512);
  EXPECT_EQ(512, PROG_MEM_SIZE);
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(fetchTests, testFetch_valid) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, &fakeMem, sizeof(fakeMem));

  reg_file[PC] = 0; // init PC at beginning of mem

  ASSERT_TRUE(fetch());
  EXPECT_EQ(0x11, cntrl_regs[OPERATION]);
  EXPECT_EQ(0x22, cntrl_regs[OPERAND_1]);
  EXPECT_EQ(0x33, cntrl_regs[OPERAND_2]);
  EXPECT_EQ(0x44, cntrl_regs[OPERAND_3]);
  EXPECT_EQ(0x04030201, cntrl_regs[IMMEDIATE]);
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(fetchTests, testFetch_outOfBounds) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  reg_file[PC] = 9; // put PC where next instruction fetch will oob

  ASSERT_FALSE(fetch());
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(decodeTests, testDecode_instMOV_valid) {
  // setup reg_file
  reg_file[R1] = 0x1234; // dest reg does not need value for this test
  reg_file[R3] = 0x5678;

  // setup instruction
  cntrl_regs[OPERATION] = MOV;
  cntrl_regs[OPERAND_1] = R1; // dest
  cntrl_regs[OPERAND_2] = R3; // src

  ASSERT_TRUE(decode());
  ASSERT_EQ(reg_file[R3], data_regs[REG_VAL_1]);
  ASSERT_EQ(0x5678, data_regs[REG_VAL_1]);
}

TEST(decodeTests, testDecode_instMOV_invalidRD) {
  // setup instruction
  cntrl_regs[OPERATION] = MOV;
  cntrl_regs[OPERAND_1] = 99; // dest, invalid reg
  cntrl_regs[OPERAND_2] = R3; // src

  ASSERT_FALSE(decode());
}

TEST(decodeTests, testDecode_instMOV_invalidRS) {
  // setup instruction
  cntrl_regs[OPERATION] = MOV;
  cntrl_regs[OPERAND_1] = R1; // dest
  cntrl_regs[OPERAND_2] = 99; // src,  invalid reg

  ASSERT_FALSE(decode());
}

TEST(decodeTests, testDecode_instMOV_invalidBoth) {
  // setup instruction
  cntrl_regs[OPERATION] = MOV;
  cntrl_regs[OPERAND_1] = 99; // dest, invalid reg
  cntrl_regs[OPERAND_2] = 99; // src,  invalid reg

  ASSERT_FALSE(decode());
}

// BEGIN TODO
// class decodeTests : public testing::TestWithParam<std::tuple<int, int, int>> {};

// TEST_P(decodeTests, testDecode) {
//     unsigned int operand_1 = std::get<1>(GetParam());
//     unsigned int operand_2 = std::get<2>(GetParam());
//     cntrl_regs[OPERATION] = std::get<0>(GetParam());
//     cntrl_regs[OPERAND_1] = operand_1;
//     cntrl_regs[OPERAND_2] = operand_2;

//     // explicit success-cases
//     bool isValid = (operand_1 == R1 && operand_2 == R2);

//     ASSERT_EQ(isValid, decode());
//   }

// INSTANTIATE_TEST_SUITE_P(
//   twoTermOps,
//   decodeTests,
//   testing::Combine(
//     testing::Values(MOV, ADDI, SUBI, MULI, DIVI),
//     testing::Values(R1, 99),
//     testing::Values(R2, 99)
//   )
// );
// END TODO: All of these tests are terrible.

TEST(executeTests, testExecute_instSTR_valid) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  data_regs[REG_VAL_1] = 0x99887766; // int value to store
  cntrl_regs[OPERATION] = STR;
  cntrl_regs[IMMEDIATE] = 0x04; // destination will be at offset 4 for this test

  ASSERT_TRUE(execute());
  // addresses are reversed because little-endian
  EXPECT_EQ(0x66, prog_mem[4]);
  EXPECT_EQ(0x77, prog_mem[5]);
  EXPECT_EQ(0x88, prog_mem[6]);
  EXPECT_EQ(0x99, prog_mem[7]);
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(executeTests, testExecute_instSTR_invalid) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  data_regs[REG_VAL_1] = 0x99887766; // value to store
  cntrl_regs[OPERATION] = STR;
  cntrl_regs[IMMEDIATE] = 0x05;

  ASSERT_FALSE(execute());
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(executeTests, testExecute_instLDR_valid) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  cntrl_regs[OPERATION] = LDR;
  cntrl_regs[OPERAND_1] = R3; // storing in to R3
  cntrl_regs[IMMEDIATE] = 0x02; // source will be at offset 2 for this test

  ASSERT_TRUE(execute());
  // addresses are reversed because little-endian
  EXPECT_EQ(0x02014433, reg_file[R3]);
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(executeTests, testExecute_instLDR_invalid) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  cntrl_regs[OPERATION] = LDR;
  cntrl_regs[OPERAND_1] = R3; // storing in to R3
  cntrl_regs[IMMEDIATE] = 0x05; // source will be at offset 2 for this test

  ASSERT_FALSE(execute());
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(integrationTests, testDecodeExecute_instSTR) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    0x11, 0x22, 0x33, 0x44, 0x01, 0x02, 0x03, 0x04
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  reg_file[R1] = 0x99887766; // int value to store
  cntrl_regs[OPERATION] = STR;
  cntrl_regs[OPERAND_1] = R1;
  cntrl_regs[IMMEDIATE] = 0x04; // destination will be at offset 4 for this test

  ASSERT_TRUE(decode());
  ASSERT_TRUE(execute());
  // addresses are reversed because little-endian
  EXPECT_EQ(0x66, prog_mem[4]);
  EXPECT_EQ(0x77, prog_mem[5]);
  EXPECT_EQ(0x88, prog_mem[6]);
  EXPECT_EQ(0x99, prog_mem[7]);
  delete[] prog_mem;
  prog_mem = nullptr;
}

TEST(integrationTests, testFetchDecodeExecute_instSTR) {
  // artificially populate memory
  const unsigned char fakeMem[] = {
    STR, R1, 0x33, 0x44, 0x03, 0x00, 0x00, 0x00
  };
  init_mem(sizeof(fakeMem));
  memcpy(prog_mem, fakeMem, sizeof(fakeMem));

  reg_file[R1] = 0x99887766;
  reg_file[PC] = 0x00;

  ASSERT_TRUE(fetch());
  ASSERT_TRUE(decode());
  ASSERT_TRUE(execute());
  // addresses are reversed because little-endian
  EXPECT_EQ(0x66, prog_mem[3]);
  EXPECT_EQ(0x77, prog_mem[4]);
  EXPECT_EQ(0x88, prog_mem[5]);
  EXPECT_EQ(0x99, prog_mem[6]);
  delete[] prog_mem;
  prog_mem = nullptr;
}