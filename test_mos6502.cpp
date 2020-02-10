
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "mos6502.h"

uint8_t ram[0xFFFF];

uint8_t MemoryRead(uint16_t address)
{
    return ram[address];
}
void MemoryWrite(uint16_t address, uint8_t value)
{
    ram[address] = value;
}

int main()
{
    std::cout << "Start TEST" <<endl;

    mos6502 cpu(MemoryRead, MemoryWrite);

    std::string sFileName = "6502_functional_test.bin";

    std::ifstream ifs;
    ifs.open(sFileName, std::ifstream::binary);
    if (ifs.is_open())
    {
        ifs.read((char*)ram, 0xFFFF);
        ifs.close();
    }

    ram[0xFFFC] = 0x00;
    ram[0xFFFD] = 0x04;

    cpu.Reset();

    uint64_t cycleCount = 0;

    cpu.Run(0x1000, cycleCount, mos6502::CYCLE_COUNT);
}

