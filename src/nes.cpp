#include "nes.hpp"

void Nes::SetROM(std::string romfile)
{
  bus.SetROM(romfile);
}

void Nes::Start()
{
  cpu.Start();
  for (int i=0x8000; i<0x9000; i++) {
    if(i%0x20==0) std::cout << std::endl;
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(static_cast<uint8_t>(bus.Read(i)))
              << ' ';
  }
  std::cout << std::endl;
}
