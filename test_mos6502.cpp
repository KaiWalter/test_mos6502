
#include <chrono>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>

#ifdef OLC
#include "olc6502.h"
#include "Bus.h"
using namespace std;
#else
#include "mos6502.h"
#endif

uint8_t ram[0x10000];
bool execute = true;
uint16_t maxpc = 0;
uint16_t prevpc = 0;

#define END_OF_MAIN 0x3469

uint8_t MemoryRead(uint16_t address)
{
	return ram[address];
}

void MemoryWrite(uint16_t address, uint8_t value)
{
	ram[address] = value;
}

void StatusWrite(uint16_t pc, uint8_t sp, uint8_t status, uint8_t A, uint8_t X, uint8_t Y, uint8_t opcode, bool illegalOpcode)
{
	if (pc > maxpc&& pc < END_OF_MAIN)
	{
		maxpc = pc;
#ifndef DETAILED
		auto givemetime = chrono::system_clock::to_time_t(chrono::system_clock::now());
		char printtime[30];
		ctime_s(printtime, 30, &givemetime);
		cout << " MAXPC:";
		cout << std::setw(4) << std::setfill('0') << std::hex << (int)maxpc << ":" << printtime;
#endif // !DETAILED

	}

	if(ram[0x1fe]== 0xd2)
		int magic = 42;

	if (pc == 0x09cf)
		int magic = 42;

	if (pc == 0x37df)
		int magic = 42;

	if (pc == END_OF_MAIN)
	{
		execute = false;
		cout << "PC:";
		cout << std::setw(4) << std::setfill('0') << std::hex << (int)pc;
		cout << " ** end of main" << endl;
	}
	else if (pc == prevpc && pc > 0x0400)
	{
		execute = false;
		cout << "PC:";
		cout << std::setw(4) << std::setfill('0') << std::hex << (int)pc;
		cout << " | Op:";
		cout << std::setw(2) << std::hex << (int)opcode;
		cout << " !! loop" << endl;
	}

	if (illegalOpcode)
		execute = false;

#ifdef DETAILED
	cout << "PC:";
	cout << std::setw(4) << std::setfill('0') << std::hex << (int)pc;
	cout << " MAXPC:";
	cout << std::setw(4) << std::setfill('0') << std::hex << (int)maxpc;
	cout << " | SP:";
	cout << std::setw(2) << std::hex << (int)sp;
	cout << " | STATUS:";
	cout << std::setw(2) << std::hex << (int)status;
	cout << " | A:";
	cout << std::setw(2) << std::hex << (int)A;
	cout << " X:";
	cout << std::setw(2) << std::hex << (int)X;
	cout << " Y:";
	cout << std::setw(2) << std::hex << (int)Y;
	cout << " | Op:";
	cout << std::setw(2) << std::hex << (int)opcode << endl;

	//if (pc == 0x3343 && ram[0x000d] > 0xFE)
	//	cout << std::setw(4) << std::setfill('0') << std::hex << (int)ram[0x000d] << "============================================================" << endl;

	//if (pc == 0x3345)
	//	cout << "============================================================" << endl;
#endif

	prevpc = pc;
}

int main()
{
	std::cout << "Start TEST" << endl;

#if OLC
	Bus bus(MemoryRead, MemoryWrite, StatusWrite);
#else
	mos6502 cpu(MemoryRead, MemoryWrite);
#endif

	std::string sFileName = "6502_functional_test.bin";

	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		ifs.read((char*)ram, 0x10000);
		ifs.close();
	}

	ram[0xFFFC] = 0x00;
	ram[0xFFFD] = 0x04;

#if OLC
	bus.reset();
#else
	cpu.Reset();
	uint64_t cycleCount = 0;
#endif


	while (execute)
	{
#if OLC
		bus.clock();
#else
		cpu.Run(1, cycleCount, StatusWrite, mos6502::INST_COUNT);
#endif
	}
}

