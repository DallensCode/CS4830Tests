#include <gtest/gtest.h>
#include <string>
#include "emu4380.h"

//Test fetch 
//     make sure that control regs are properly read
//     make sure that bounds check works
//     make sure that PC was incremented properly
TEST( FectchTests, AddressCheck){
	init_mem(16);
	unsigned char testInstruction[] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	prog_mem = testInstruction;

	reg_file[PC] = 0;
	EXPECT_EQ( fetch(),true)<<"Fetch Fail";
	reg_file[PC] = 5;
	EXPECT_EQ( fetch(),true)<<"Fetch Fail";
	reg_file[PC] = 6;
	EXPECT_EQ( fetch(),true)<<"Fetch Fail";
	reg_file[PC] = 8;
	EXPECT_EQ( fetch(),true)<<"Fetch fail";//Not enough bytes for instruction
	reg_file[PC] = 9;
	EXPECT_EQ( fetch(),false)<<"Fetch Worked but should fail";
	reg_file[PC] = 10;
	EXPECT_EQ( fetch(),false)<<"Fetch Worked but should fail";
	reg_file[PC] = 20;
	EXPECT_EQ( fetch(),false)<<"Fetch Worked but should fail";
	//delete[] prog_mem;
}
TEST( FectchTests, ControlRegs){
	init_mem(16);
	unsigned char testInstructions[] = {1,2,3,4,0,0,0,1,9,10,11,12,1,0,0,0};
	prog_mem = testInstructions;

	reg_file[PC] = 0;
	bool res = fetch();
	EXPECT_EQ(res,true)<<"fetch failed but should work";//Something went wrong!
	EXPECT_EQ(cntrl_regs[OPERATION],1)<<"fetch operation incorrect";
	EXPECT_EQ(cntrl_regs[OPERAND_1],2)<<"fetch operand 1 incorrect";
	EXPECT_EQ(cntrl_regs[OPERAND_2],3)<<"fetch operand 2 incorrect";
	EXPECT_EQ(cntrl_regs[OPERAND_3],4)<<"fetch operand 3 incorrect";
	EXPECT_EQ(cntrl_regs[IMMEDIATE],1<<24)<<"fetch immediate incorrect";
	
	EXPECT_EQ(reg_file[PC],8)<<"PC not incremented after fetch";
	res = fetch();//Test if incremented PC too
	EXPECT_EQ(reg_file[PC],16)<<"PC not incremented after fetch";
	EXPECT_EQ(res,true)<<"fetch failed but should work";//Something went wrong!
	EXPECT_EQ(cntrl_regs[OPERATION],9) <<"fetch operation incorrect";
	EXPECT_EQ(cntrl_regs[OPERAND_1],10)<<"fetch operand 1 incorrect";
	EXPECT_EQ(cntrl_regs[OPERAND_2],11)<<"fetch operand 2 incorrect";
	EXPECT_EQ(cntrl_regs[OPERAND_3],12)<<"fetch operand 3 incorrect";
	EXPECT_EQ(cntrl_regs[IMMEDIATE],1)<<"fetch immediate incorrect";
	
	//delete[] prog_mem;
}


//Test decode
//     make sure that invalid instructions are caught
//     make sure that all valid instructions are accepted
//     make sure out bound registers are caught
//     make sure that data_regs are set properly for operand
TEST( DecodeTests, invalidOperations){
	unsigned int invalidOperations[] = {0,2,3,4,5,6,14,15,16,17,27,28,29,300,400,600};
	for( int i = 0; i < 15; ++i){
		cntrl_regs[OPERATION] = invalidOperations[i];
		EXPECT_EQ(decode(),false)<<"Invalid instruction was not caught";
	}
}

TEST( DecodeTests, RegChecks){
	unsigned int oneRegs[] = {8,9,10,11,12,13};
	for( int i = 0; i < 6; ++i){
		cntrl_regs[OPERATION] = oneRegs[i];
		cntrl_regs[OPERAND_1] = 30;
		cntrl_regs[OPERAND_2] = 30;
		cntrl_regs[OPERAND_3] = 30;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),false)<<"Invalid register was not caught";
		cntrl_regs[OPERAND_1] = 21;
		cntrl_regs[OPERAND_2] = 100;
		cntrl_regs[OPERAND_3] = 100;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),true)<<"Valid instruction was caught";
	}
	unsigned int twoRegs[] = {7,19,21,23,26};
	for( int i = 0; i < 5; ++i){
		cntrl_regs[OPERATION] = twoRegs[i];
		cntrl_regs[OPERAND_1] = 30;
		cntrl_regs[OPERAND_2] = 30;
		cntrl_regs[OPERAND_3] = 1000;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),false)<<"Invalid register was not caught";
		cntrl_regs[OPERAND_1] = 10;
		cntrl_regs[OPERAND_2] = 11;
		cntrl_regs[OPERAND_3] = 30;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),true)<<"Valid instruction was caught";
	}
	unsigned int threeRegs[] = {18,20,22,24,25};
	for( int i = 0; i < 5; ++i){
		cntrl_regs[OPERATION] = threeRegs[i];
		cntrl_regs[OPERAND_1] = 30;
		cntrl_regs[OPERAND_2] = 30;
		cntrl_regs[OPERAND_3] = 1000;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),false)<<"Invalid register was not caught";
		cntrl_regs[OPERAND_1] = 10;
		cntrl_regs[OPERAND_2] = 11;
		cntrl_regs[OPERAND_3] = 0;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),true)<<"Valid instruction was caught";
	}
}

TEST( DecodeTests, TrapCodes){
	unsigned int validTrapCodes[] = {0,1,2,3,4,98};
	
	for( int i = 0; i < 6; ++i){
		cntrl_regs[OPERATION] = 31;
		cntrl_regs[OPERAND_1] = 30;
		cntrl_regs[OPERAND_2] = 30;
		cntrl_regs[OPERAND_3] = 30;
		cntrl_regs[IMMEDIATE] = validTrapCodes[i];
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),true)<<"Valid trap code was caught";
	}

	unsigned int someInvalidTrapCodes[] = {5,10,34,50,100};
	for( int i = 0; i < 5; ++i){
		cntrl_regs[OPERATION] = 31;
		cntrl_regs[OPERAND_1] = 30;
		cntrl_regs[OPERAND_2] = 30;
		cntrl_regs[OPERAND_3] = 30;
		cntrl_regs[IMMEDIATE] = someInvalidTrapCodes[i];
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),false)<<"Invalid trap code was not caught";
	}
}

TEST( DecodeTests, ValidInstructions){
	//Test
	unsigned int validInstructions[] = {1,7,8,9,10,11,12,13,18,19,20,21,22,23,24,25,26,31};
	for( int i = 0; i < 18; ++i){
		cntrl_regs[OPERATION] = validInstructions[i];
		cntrl_regs[OPERAND_1] = 1;
		cntrl_regs[OPERAND_2] = 0;
		cntrl_regs[OPERAND_3] = 0;
		cntrl_regs[IMMEDIATE] = 0;
		data_regs[0] = 0;
		data_regs[1] = 0;
		EXPECT_EQ(decode(),true)<<"Valid Instruction was not decoded";
	}
}

TEST( ExecuteTests, IllegalOperations){
	init_mem(10);//This is to check for illegal addresses

	//Test jmp
	cntrl_regs[OPERATION] = 1;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 0;
	cntrl_regs[OPERAND_3] = 0;
	cntrl_regs[IMMEDIATE] = 11;
	data_regs[0] = 0;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//STR
	cntrl_regs[OPERATION] = 10;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 0;
	cntrl_regs[OPERAND_3] = 0;
	cntrl_regs[IMMEDIATE] = 10;
	data_regs[0] = 0;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	cntrl_regs[OPERATION] = 10;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 0;
	cntrl_regs[OPERAND_3] = 0;
	cntrl_regs[IMMEDIATE] = 7;//There only 3bytes
	data_regs[0] = 0;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//LDR
	cntrl_regs[OPERATION] = 11;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 0;
	cntrl_regs[OPERAND_3] = 0;
	cntrl_regs[IMMEDIATE] = 10;
	data_regs[0] = 0;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//STB
	cntrl_regs[OPERATION] = 12;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 0;
	cntrl_regs[OPERAND_3] = 0;
	cntrl_regs[IMMEDIATE] = 15;
	data_regs[0] = 0;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//LDB
	cntrl_regs[OPERATION] = 13;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 0;
	cntrl_regs[OPERAND_3] = 0;
	cntrl_regs[IMMEDIATE] = 15;
	data_regs[0] = 0;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//DIV
	cntrl_regs[OPERATION] = 24;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 15;
	cntrl_regs[OPERAND_3] = 43;
	cntrl_regs[IMMEDIATE] = 15;
	data_regs[0] = 10;
	data_regs[1] = 0;//This is divisor
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//SDIV
	cntrl_regs[OPERATION] = 25;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 15;
	cntrl_regs[OPERAND_3] = 43;
	cntrl_regs[IMMEDIATE] = 15;
	data_regs[0] = -10;
	data_regs[1] = 0;//This is divisor
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
	//DIVI
	cntrl_regs[OPERATION] = 25;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[OPERAND_2] = 15;
	cntrl_regs[OPERAND_3] = 43;
	cntrl_regs[IMMEDIATE] = 0;
	data_regs[0] = -10;
	data_regs[1] = 0;
	EXPECT_EQ(execute(),false)<<"Illegal Operation was run";
}

TEST( ExecuteTests, TestJMP){
	init_mem(120);//This is for address check
	//JMP doesn't check for address currently
	cntrl_regs[OPERATION] = 1;
	cntrl_regs[OPERAND_1] = 1353;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 100;
	data_regs[0] = -10;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ(reg_file[PC],100);
}

TEST( ExecuteTests, TestMoves){
	cntrl_regs[OPERATION] = 7;
	cntrl_regs[OPERAND_1] = 5;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 100;
	data_regs[0] = 2999;//This is taken from operand 2
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ( reg_file[R5],2999);

	cntrl_regs[OPERATION] = 8;
	cntrl_regs[OPERAND_1] = 0;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 30990;
	data_regs[0] = 2999;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ( reg_file[R0],30990);
}

TEST( ExecuteTests, TestLoadsAndStores){
	//LDA
	cntrl_regs[OPERATION] = 9;
	cntrl_regs[OPERAND_1] = 2;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 6424;
	data_regs[0] = 29990;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ( reg_file[R2],6424);

	//STR
	init_mem(50);

	cntrl_regs[OPERATION] = 10;
	cntrl_regs[OPERAND_1] = 3;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 34;//Store address
	data_regs[0] = 0x01020304;//This is reg value from OPERAND_1
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ( prog_mem[34], 4);
	EXPECT_EQ( prog_mem[35], 3);
	EXPECT_EQ( prog_mem[36], 2);
	EXPECT_EQ( prog_mem[37], 1);
	//Check if the bytes were reversed
	EXPECT_NE( prog_mem[34], 1);
	EXPECT_NE( prog_mem[35], 2);
	EXPECT_NE( prog_mem[36], 3);
	EXPECT_NE( prog_mem[37], 4);

	//LDR
	cntrl_regs[OPERATION] = 11;
	cntrl_regs[OPERAND_1] = 4;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 34;//Load address from previous instruction
	data_regs[0] = 3342345;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ(reg_file[R4],0x01020304);

	//Set up some memory
	prog_mem[10]=10;
	prog_mem[11]=11;
	prog_mem[12]=12;
	prog_mem[13]=13;
	cntrl_regs[OPERATION] = 11;
	cntrl_regs[OPERAND_1] = 6;//Put integer in R6
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 10;//Load address
	data_regs[0] = 0x01020304;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ(reg_file[R6],0x0D0C0B0A);
	//STB
	cntrl_regs[OPERATION] = 12;
	cntrl_regs[OPERAND_1] = 7;//this isn't used
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;//store address
	data_regs[0] = 'b';//This is set to where OPERAND_1 says
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ(prog_mem[4],'b');
	//LDB
	cntrl_regs[OPERATION] = 13;
	cntrl_regs[OPERAND_1] = 8;//Put char in R8
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;//load address
	data_regs[0] = 42;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ(reg_file[R8],'b');
	
	prog_mem[49]='q';
	cntrl_regs[OPERATION] = 13;
	cntrl_regs[OPERAND_1] = 9;//Put char in R9
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 49;//load address
	data_regs[0] = 42;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	EXPECT_EQ(reg_file[R9],'q');
}
TEST( ExecuteTests, TestArithmetic){
	//ADD
	cntrl_regs[OPERATION] = 18;
	cntrl_regs[OPERAND_1] = 10;//Put result in R10
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 49;
	data_regs[0] = 42;
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R10],42+101);

	cntrl_regs[OPERATION] = 18;
	cntrl_regs[OPERAND_1] = 10;//Put result in R10
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 49;
	data_regs[0] = 53;
	data_regs[1] = -105;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R10],53-105);
	//ADDI
	cntrl_regs[OPERATION] = 19;
	cntrl_regs[OPERAND_1] = 11;//Put result in R11
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 1000;
	data_regs[0] = 42;//This should be set to whatever Operand 2 says
	data_regs[1] = 101;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R11],42+1000);
	//SUB
	cntrl_regs[OPERATION] = 20;
	cntrl_regs[OPERAND_1] = 12;//Put result in R12
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 1000;
	data_regs[0] = 100;//This is set to whatever Operand 2 says
	data_regs[1] = 60;//This is set to whatever Operand 3 says
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R12],100-60);
	cntrl_regs[OPERATION] = 20;
	cntrl_regs[OPERAND_1] = 12;//Put result in R12
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 1000;
	data_regs[0] = 13;//This is set to whatever Operand 2 says
	data_regs[1] = 61;//This is set to whatever Operand 3 says
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R12],13-61);
	//SUBI
	cntrl_regs[OPERATION] = 21;
	cntrl_regs[OPERAND_1] = 13;//Put result in R13
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;
	data_regs[0] = 100;//This is set to whatever Operand 2 says
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R13],100-4);
	
	//MUL
	cntrl_regs[OPERATION] = 22;
	cntrl_regs[OPERAND_1] = 14;//Put result in R14
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;
	data_regs[0] = 100;//This is set to whatever Operand 2 says
	data_regs[1] = 60;//This is set to whatever operand 3 says
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R14],60*100);
	//MULI
	cntrl_regs[OPERATION] = 23;
	cntrl_regs[OPERAND_1] = 15;//Put result in R15
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;
	data_regs[0] = 100;//This is set to whatever Operand 2 says
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R15],400);
	//DIV
	cntrl_regs[OPERATION] = 24;
	cntrl_regs[OPERAND_1] = 14;//Put result in R14
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;
	data_regs[0] = 101;//This is set to whatever Operand 2 says
	data_regs[1] = 60;//This is set to whatever operand 3 says
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R14],101/(unsigned int)60);
	//SDIV
	cntrl_regs[OPERATION] = 25;
	cntrl_regs[OPERAND_1] = 15;//Put result in R15
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;
	data_regs[0] = -100;//This is set to whatever Operand 2 says
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	int result = -100/60;
	void* resPtr = &result;
	unsigned int uResult = *((unsigned int*)resPtr);
	ASSERT_EQ(reg_file[R15],uResult);
	ASSERT_EQ(reg_file[R15],-1);

	cntrl_regs[OPERATION] = 25;
	cntrl_regs[OPERAND_1] = 15;//Put result in R15
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;
	data_regs[0] = 150;//This is set to whatever Operand 2 says
	data_regs[1] = -60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	result = (int)150/(-60);
	resPtr = &result;
	uResult = *((unsigned int*)resPtr);
	ASSERT_EQ(reg_file[R15],uResult);
	ASSERT_EQ(reg_file[R15],-2);
	//DIVI
	cntrl_regs[OPERATION] = 26;
	cntrl_regs[OPERAND_1] = 15;//Put result in R15
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 443;
	data_regs[0] = 10*443;//This is set to whatever Operand 2 says
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R15],10);
}
TEST( ExecuteTests, TestTraps){
	//Stdin setup
	int fildes[2];
	int status = pipe(fildes);
	ASSERT_NE(status,-1)<<"Problem setting up stdin for Trap 2";
	status = dup2(fildes[0], STDIN_FILENO);
	ASSERT_NE(status,-1)<<"Problem setting up stdin for Trap 2";

	//Can't really test trap 0
	//Trap 1
	testing::internal::CaptureStdout();

	reg_file[R3]=0;
	cntrl_regs[OPERATION] = 31;
	cntrl_regs[OPERAND_1] = 15;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 1;//Trap 1
	data_regs[0] = 10*443;//This should has been set to whatever R3 contained
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output,"4430")<<"TRP 1 printed incorrectly";
	//Trap 2
	char buf[] = "201\n";
	ssize_t nbytes = write(fildes[1],buf,4);

	cntrl_regs[OPERATION] = 31;
	cntrl_regs[OPERAND_1] = 15;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 2;//Trap 2
	data_regs[0] = 10*443;
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R3],201)<<"Trap2 did not take input correctly";
	
	//Trap 3
	testing::internal::CaptureStdout();

	cntrl_regs[OPERATION] = 31;
	cntrl_regs[OPERAND_1] = 15;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 3;//Trap 3
	data_regs[0] = 'g';//This should has been set to whatever R3 contained
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output,"g")<<"TRP 3 printed incorrectly";
	
	//Trap 4
	char buf2[] = "xyzabc\n";
	nbytes = write(fildes[1],buf2,7);

	cntrl_regs[OPERATION] = 31;
	cntrl_regs[OPERAND_1] = 15;
	cntrl_regs[OPERAND_2] = 15641;
	cntrl_regs[OPERAND_3] = 4309;
	cntrl_regs[IMMEDIATE] = 4;//Trap 4
	data_regs[0] = 10*443;
	data_regs[1] = 60;
	EXPECT_EQ(execute(),true)<<"Execute should have worked";
	ASSERT_EQ(reg_file[R3],'x')<<"Trap4 did not take input correctly";
	//Trap 5 isn't easy to test
}
