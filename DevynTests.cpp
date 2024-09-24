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

TEST(EMU4380_UNITTESTS, MOV_TEST_VALID) {

    // setup
    unsigned char mov_instr[8] = {
        MOV, R0, R1, 00, 00, 00, 00, 00  // MOV instruction
    };
    reg_file[R0] = 0;
    reg_file[R1] = 60;

    // fetch
    cntrl_regs[OPERATION] = mov_instr[0];
    cntrl_regs[OPERAND_1] = mov_instr[1];
    cntrl_regs[OPERAND_2] = mov_instr[2];
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

TEST(EMU4380_UNITTESTS, MOVI_TEST_VALID) {

}

TEST(EMU4380_UNITTESTS, ADD_TEST_VALID) {

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


TEST(EMU4380_UNITTESTS, FETCH_ADD_VALID) {

    // test instruction
    unsigned char add_instr[8] = {
        ADD, 5, 6, 12, 0x00, 0x00, 0x00, 0x00
    };

    // setup memory
    reg_file[PC] = 0;
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, add_instr, sizeof(add_instr));

    // fetch
    EXPECT_TRUE(fetch());
    delete[] prog_mem;

    // decode
    // execute

    // assert
    EXPECT_EQ(cntrl_regs[OPERATION], ADD);
    EXPECT_EQ(cntrl_regs[OPERAND_1], 5);
    EXPECT_EQ(cntrl_regs[OPERAND_2], 6);
    EXPECT_EQ(cntrl_regs[OPERAND_3], 12);
    EXPECT_EQ(cntrl_regs[IMMEDIATE], 0x00000000);
    EXPECT_EQ(reg_file[PC], 8);
}

TEST(EMU4380_UNITTESTS, FETCH_ADD_INVALID) {

    // test instruction
    unsigned char add_instr[8] = {
        ADD, 2, 12, 12, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 6;

    // setup memory
    mem_size = 8;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, add_instr, sizeof(add_instr));

    // fetch
    EXPECT_FALSE(fetch());
    delete[] prog_mem;

    // assert
    EXPECT_EQ(reg_file[PC], 6);
}

TEST(EMU4380_UNITTESTS, DECODE_MOV_VALID) {
    


    // test instruction
    unsigned char mov_instr[8] = {
        MOV, 5, 6, 0, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;
    reg_file[R5] = 0;
    reg_file[R6] = 60;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, mov_instr, sizeof(mov_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);

    delete[] prog_mem;
    // decode
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 60);
}

TEST(EMU4380_UNITTESTS, DECODE_MOV_INVALID) {

    // test instruction
    unsigned char mov_instr[8] = {
        MOV, 0x1B, 0x1A, 0, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, mov_instr, sizeof(mov_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode
    EXPECT_FALSE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_JMP_VALID) {

    // test instruction
    unsigned char jmp_instr[8] = {
        JMP, 0, 0, 0, 0xFF, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, jmp_instr, sizeof(jmp_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    // decode
    EXPECT_TRUE(decode());       
}

TEST(EMU4380_UNITTESTS, DECODE_JMP_INVALID) {

    // test instruction
    unsigned char jmp_instr[8] = {
        JMP, 0, 0, 0, 0x00, 0xFF, 0x00, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, jmp_instr, sizeof(jmp_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    // decode
    EXPECT_FALSE(decode());  
}

TEST(EMU4380_UNITTESTS, DECODE_LDA_VALID) {
    // test instruction
    unsigned char lda_instr[8] = {
        LDA, 1, 0, 0, 0x04, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, lda_instr, sizeof(lda_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    // decode
    EXPECT_TRUE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_LDA_INVALID) {
    // test instruction
    unsigned char lda_instr[8] = {
        LDA, 1, 0, 0, 0x00, 0xFF, 0xFF, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 12;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, lda_instr, sizeof(lda_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    // decode
    EXPECT_FALSE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_STR_VALID) {
    // test instruction
    unsigned char str_instr[8] = {
        STR, 1, 0, 0, 0x04, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, str_instr, sizeof(str_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    // decode
    EXPECT_TRUE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_STR_INVALID) {
    // test instruction
    unsigned char str_instr[8] = {
        STR, 1, 0, 0, 0x09, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 12;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, str_instr, sizeof(str_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_FALSE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_LDR_VALID) {
    // test instruction
    unsigned char ldr_instr[8] = {
        LDR, 1, 0, 0, 0x09, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 1024;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, ldr_instr, sizeof(ldr_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_TRUE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_LDR_INVALID) {
    // test instruction
    unsigned char ldr_instr[8] = {
        LDR, 1, 0, 0, 0x09, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 12;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, ldr_instr, sizeof(ldr_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_FALSE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_STB_VALID) {
    // test instruction
    unsigned char stb_instr[8] = {
        STB, 1, 0, 0, 0x09, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 9;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, stb_instr, sizeof(stb_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_TRUE(decode());
}


TEST(EMU4380_UNITTESTS, DECODE_STB_INVALID) {
    // test instruction
    unsigned char stb_instr[8] = {
        STB, 1, 0, 0, 0x08, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 7;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, stb_instr, sizeof(stb_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_FALSE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_LDB_VALID) {
    // test instruction
    unsigned char ldb_instr[8] = {
        STB, 1, 0, 0, 0x09, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 9;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, ldb_instr, sizeof(ldb_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_TRUE(decode());
}


TEST(EMU4380_UNITTESTS, DECODE_LDB_INVALID) {
    // test instruction
    unsigned char ldb_instr[8] = {
        STB, 1, 0, 0, 0x08, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 7;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, ldb_instr, sizeof(ldb_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    //std::cout << "in str invalid " << cntrl_regs[IMMEDIATE] << std::endl;

    // decode
    EXPECT_FALSE(decode());
}

TEST(EMU4380_UNITTESTS, DECODE_ADD_VALID) {
    
    // test instruction
    unsigned char add_instr[8] = {
        ADD, 1, 2, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 40;
    reg_file[R3] = 60;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, add_instr, sizeof(add_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode/assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 40);
    EXPECT_EQ(data_regs[REG_VAL_2], 60);
}

TEST(EMU4380_UNITTESTS, DECODE_ADD_INVALID) {

    // test instruction
    unsigned char add_instr[8] = {
        ADD, 1, 50, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, add_instr, sizeof(add_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}


TEST(EMU4380_UNITTESTS, DECODE_ADDI_VALID) {

    // test instruction
    unsigned char addi_instr[8] = {
        ADDI, 1, 2, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, addi_instr, sizeof(addi_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 30);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_ADDI_INVALID) {

    // test instruction
    unsigned char addi_instr[8] = {
        ADDI, 1, 23, 3, 0x00, 0xFF, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 30;
    reg_file[R2] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, addi_instr, sizeof(addi_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_SUB_VALID) {

    // test instruction
    unsigned char sub_instr[8] = {
        SUB, 1, 15, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R15] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, sub_instr, sizeof(sub_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 30);
    EXPECT_EQ(data_regs[REG_VAL_2], 40);
}

TEST(EMU4380_UNITTESTS, DECODE_SUB_INVALID) {

    // test instruction
    unsigned char sub_instr[8] = {
        SUB, 1, 23, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 30;
    reg_file[R2] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, sub_instr, sizeof(sub_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_SUBI_VALID) {

    // test instruction
    unsigned char subi_instr[8] = {
        SUBI, 1, 15, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R15] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, subi_instr, sizeof(subi_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 30);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_SUBI_INVALID) {

    // test instruction
    unsigned char subi_instr[8] = {
        SUBI, 1, 23, 3, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 30;
    reg_file[R2] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, subi_instr, sizeof(subi_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_MUL_VALID) {

    // test instruction
    unsigned char mul_instr[8] = {
        MUL, 1, 15, 5, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R15] = 30;
    reg_file[R5] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, mul_instr, sizeof(mul_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 30);
    EXPECT_EQ(data_regs[REG_VAL_2], 40);
}

TEST(EMU4380_UNITTESTS, DECODE_MUL_INVALID) {

    // test instruction
    unsigned char mul_instr[8] = {
        MUL, 1, 2, 23, 0x00, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, mul_instr, sizeof(mul_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_DIV_VALID) {

    // test instruction
    unsigned char div_instr[8] = {
        DIV, 1, 2, 5, 0x09, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R5] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, div_instr, sizeof(div_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 30);
    EXPECT_EQ(data_regs[REG_VAL_2], 40);
}

TEST(EMU4380_UNITTESTS, DECODE_DIV_INVALID) {

    // test instruction
    unsigned char div_instr[8] = {
        DIV, 1, 23, 45, 0xFF, 0xFF, 0xFF, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, div_instr, sizeof(div_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_SDIV_VALID) {

    // test instruction
    unsigned char sdiv_instr[8] = {
        SDIV, 1, 2, 5, 0xFF, 0xFF, 0xFF, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R5] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, sdiv_instr, sizeof(sdiv_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);

    delete[] prog_mem;
    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 30);
    EXPECT_EQ(data_regs[REG_VAL_2], 40);
}

TEST(EMU4380_UNITTESTS, DECODE_SDIV_INVALID) {

    // test instruction
    unsigned char sdiv_instr[8] = {
        SDIV, 1, 23, 45, 0xFF, 0xFF, 0xFF, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, sdiv_instr, sizeof(sdiv_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);

    delete[] prog_mem;
    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_DIVI_VALID) {

    // test instruction
    unsigned char divi_instr[8] = {
        DIVI, 1, 2, 5, 0x80, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 50;
    reg_file[R5] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, divi_instr, sizeof(divi_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;
    // decode and assert
    EXPECT_TRUE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 50);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}

TEST(EMU4380_UNITTESTS, DECODE_DIVI_INVALID) {

    // test instruction
    unsigned char divi_instr[8] = {
        DIVI, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R2] = 30;
    reg_file[R3] = 40;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 128;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, divi_instr, sizeof(divi_instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);

    delete[] prog_mem;
    // decode and assert
    EXPECT_FALSE(decode());
    EXPECT_EQ(data_regs[REG_VAL_1], 0);
    EXPECT_EQ(data_regs[REG_VAL_2], 0);
}


TEST(EMU4380_UNITTESTS, EXECUTE_JMP_VALID) {

    // test instruction
    unsigned char instr[8] = {
        JMP, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 4294967295;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, instr, sizeof(instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // execute and assert
    EXPECT_TRUE(execute());
    EXPECT_EQ(reg_file[PC], 4294967295);
}

TEST(EMU4380_UNITTESTS, EXECUTE_JMP_INVALID) {

    // test instruction
    unsigned char instr[8] = {
        JMP, 1, 2, 3, 0x96, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[PC] = 0;

    // setup memory
    mem_size = 149;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, instr, sizeof(instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // execute and assert
    EXPECT_FALSE(execute());
    EXPECT_NE(reg_file[PC], 150);
}

TEST(EMU4380_UNITTESTS, EXECUTE_MOV_VALID) {

    // test instruction
    unsigned char instr[8] = {
        MOV, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 0;
    reg_file[R2] = 4294967295;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 4294967295;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, instr, sizeof(instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode
    data_regs[REG_VAL_1] = reg_file[R2];

    // execute and assert
    EXPECT_TRUE(execute());
    EXPECT_EQ(reg_file[R1], 4294967295);
    EXPECT_EQ(data_regs[REG_VAL_1], 4294967295);

}

TEST(EMU4380_UNITTESTS, EXECUTE_MOV_INVALID) {

    // test instruction
    unsigned char instr[8] = {
        MOV, 1, 2, 3, 0x96, 0x00, 0x00, 0x00
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 0;
    reg_file[R2] = 140;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 149;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, instr, sizeof(instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // execute and assert
    execute();
    EXPECT_NE(reg_file[R1], 150);
    EXPECT_NE(data_regs[REG_VAL_1], 150);
}

TEST(EMU4380_UNITTESTS, EXECUTE_MOVI_VALID) {

    // test instruction
    unsigned char instr[8] = {
        MOVI, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 0;
    reg_file[R2] = 4294967295;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 4294967295;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, instr, sizeof(instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // execute and assert
    EXPECT_TRUE(execute());
    EXPECT_EQ(reg_file[R1], 4294967295);
    EXPECT_NE(data_regs[REG_VAL_1], 4294967295);

}

TEST(EMU4380_UNITTESTS, EXECUTE_MOVI_INVALID) {

    // test instruction
    unsigned char instr[8] = {
        MOVI, 1, 2, 3, 0xFF, 0xFF, 0xFF, 0xFF
    };

    // setup registers
    std::memset(data_regs, 0, sizeof(data_regs));
    std::memset(cntrl_regs, 0, sizeof(cntrl_regs));
    std::memset(reg_file, 0, sizeof(reg_file));
    reg_file[R1] = 0;
    reg_file[R2] = 140;
    reg_file[PC] = 0;

    // setup memory
    mem_size = 149;
    prog_mem = new unsigned char[mem_size];
    std::memcpy(prog_mem, instr, sizeof(instr));

    // fetch
    cntrl_regs[OPERATION] = prog_mem[reg_file[PC]];
    cntrl_regs[OPERAND_1] = prog_mem[reg_file[PC] + 1];
    cntrl_regs[OPERAND_2] = prog_mem[reg_file[PC] + 2];
    cntrl_regs[OPERAND_3] = prog_mem[reg_file[PC] + 3];
    cntrl_regs[IMMEDIATE] = static_cast<unsigned int>(prog_mem[reg_file[PC] + 4]) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 5]) << 8) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 6]) << 16) |
                    (static_cast<unsigned int>(prog_mem[reg_file[PC] + 7]) << 24);
    delete[] prog_mem;

    // decode and assert
    execute();
    EXPECT_NE(reg_file[R1], 255);
    EXPECT_NE(data_regs[REG_VAL_1], 255);
}

int runMain(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
