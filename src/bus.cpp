#include "bus.hpp"

Bus::Bus(Ram& ram, Rom& rom)
  : ram_(ram), rom_(rom)
{
}

uint8_t Bus::Read(uint16_t addr) const
{
  if(addr >= 0x0000 && addr <= 0x07FF){
    return ram_.Read(addr);
  }
  else if(addr >= 0x8000){
    return rom_.Read(addr-0x8000);
  }
  else {
    return ram_.Read(addr);
  }
}

void Bus::Write(uint16_t addr, uint8_t data)
{
  if(addr >= 0x0000 && addr <= 0x07FF){
    ram_.Write(addr, data);
  }
}

void Bus::SetROM(std::string romfile)
{
  // rom_.Set(romfile);
}
