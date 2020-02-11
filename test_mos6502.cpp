
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>

#include "mos6502.h"

uint8_t ram[0x10000];
bool execute = true;
uint16_t maxpc = 0;
uint16_t minpc = 0xFFFF;

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
	if (pc > maxpc)
		maxpc = pc;

	if (pc < minpc)
		minpc = pc;

	if (opcode == 0x40 || opcode == 0x60)
	{
		maxpc = 0;
		minpc = 0xFFFF;
	}

	cout << "PC:";
	cout << std::setw(4) << std::setfill('0') << std::hex << (int)pc;
	cout << " MINPC:";
	cout << std::setw(4) << std::setfill('0') << std::hex << (int)minpc;
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
	execute = !illegalOpcode;
}

int main()
{
	std::cout << "Start TEST" << endl;

	mos6502 cpu(MemoryRead, MemoryWrite);

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

	cpu.Reset();

	uint64_t cycleCount = 0;

	while (execute)
	{
		cpu.Run(1, cycleCount, StatusWrite, mos6502::INST_COUNT);
	}

}

