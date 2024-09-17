#include <gtest/gtest.h>
#include "emu4380.h"

TEST(EMU4380_UNITTESTS, BAD_MEM_INIT) { // should fail, in progress
    EXPECT_FALSE(init_mem(static_cast<unsigned int>(345634563456ULL)));
    delete[] prog_mem;
}

TEST(EMU4380_UNITTESTS, GOOD_MEM_INIT) {
    EXPECT_TRUE(init_mem(1));
    delete[] prog_mem;
}

TEST(EMU4380_UNITTESTS, NEGATIVE_MEM_INIT) { // should fail, in progress
    //std::cout << static_cast<unsigned int>(-1) << ' ' << MAXIMUM_MEM_SIZE << std::endl;
    EXPECT_FALSE(init_mem(static_cast<unsigned int>(-1)));
    delete[] prog_mem;
}

TEST(EMU4380_UNITTESTS, MAXIMUM_MEM_INIT) {
    EXPECT_TRUE(init_mem(MAXIMUM_MEM_SIZE));
    delete[] prog_mem;
}

TEST(EMU4380_UNITTESTS, MOVTEST) {

    // setup
    unsigned char add_instr[8] = {
        MOV, R0, R1, 00, 00, 00, 00, 00  // MOV instruction
    };
    reg_file[R0] = 0;
    reg_file[R1] = 60;

    // fetch
    cntrl_regs[OPERATION] = add_instr[0];
    cntrl_regs[OPERAND_1] = add_instr[1];
    cntrl_regs[OPERAND_2] = add_instr[2];
    cntrl_regs[OPERAND_3] = 0;//DC
    cntrl_regs[IMMEDIATE] = 0;//DC

    // decode
    data_regs[REG_VAL_1] = reg_file[cntrl_regs[OPERAND_2]];

    // execute
    reg_file[cntrl_regs[OPERAND_1]] = data_regs[REG_VAL_1];

    // assert
    EXPECT_EQ(reg_file[R0], 60);
    EXPECT_EQ(reg_file[R1], 60);
}

TEST(EMU4380_UNITTESTS, ADDTEST) {

    // setup memory
    reg_file[R3] = 13;
    reg_file[R2] = 12;

    // test instruction
    unsigned char add_instr[8] = {
        ADD, R0, R3, R2, 0x00, 0x00, 0x00, 0x00  // ADD instruction
    };

    // fetch
    cntrl_regs[OPERATION] = add_instr[0];
    cntrl_regs[OPERAND_1] = add_instr[1];
    cntrl_regs[OPERAND_2] = add_instr[2];
    cntrl_regs[OPERAND_3] = add_instr[3];
    cntrl_regs[IMMEDIATE] = 0;//DC

    // decode
    data_regs[REG_VAL_1] = reg_file[cntrl_regs[OPERAND_2]];
    data_regs[REG_VAL_2] = reg_file[cntrl_regs[OPERAND_3]];

    // execute
    reg_file[cntrl_regs[OPERAND_1]] = data_regs[REG_VAL_1] + data_regs[REG_VAL_2];

    // assert
    ASSERT_EQ(reg_file[R0], 25);
}

TEST(EMU4380_UNITTESTS, FETCHTEST) {

    // setup memory
    reg_file[R3] = 13;
    reg_file[R2] = 12;

    // test instruction
    unsigned char add_instr[8] = {
        ADD, R0, R3, R2, 0x00, 0x00, 0x00, 0x00
    };

    reg_file[PC] = 0;  // Set PC to point to the start of memory (address 0)
    prog_mem = new unsigned char[1024];
    std::memcpy(prog_mem, add_instr, sizeof(add_instr));

    EXPECT_TRUE(fetch());
    delete[] prog_mem;

    EXPECT_EQ(cntrl_regs[OPERATION], ADD);
    EXPECT_EQ(cntrl_regs[OPERAND_1], R0);
    EXPECT_EQ(cntrl_regs[OPERAND_2], R3);
    EXPECT_EQ(cntrl_regs[OPERAND_3], R2);
    EXPECT_EQ(cntrl_regs[IMMEDIATE], 0x00000000);
    EXPECT_EQ(reg_file[PC], 8);
}

TEST(EMU4380_INTEGRATEDTEST, ADDTEST) { // not done

    ASSERT_TRUE(init_mem(DEFAULT_MEM_SIZE));

    unsigned char instr[8] = {
        ADD, R3, R1, R2, 0x00, 0x00, 0x00, 0x00
    };

    memcpy(prog_mem, instr, sizeof(instr));

    data_regs[REG_VAL_1] = 0;
    data_regs[REG_VAL_2] = 0;

    cntrl_regs[0] = MOV; //MOV
    cntrl_regs[1] = R3;
    cntrl_regs[2] = R4;
    cntrl_regs[3] = 55;//DC
    cntrl_regs[4] = 7;//DC

    delete[] prog_mem;
}

TEST(EMU4380_INTEGRATEDTEST, FETCHTEST) { // not done
    unsigned char instr[8] = {
        MOV, R3, R4, 0xDD, 0xEE, 0xFF, 0x00, 0x11
    };
    //ASSERT_EQ(cntrl_regs[R3], 4);
}

int runMain(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}