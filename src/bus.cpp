#include "bus.hpp"

Bus::Bus(Ram& ram, Rom& rom)
  : ram_(ram), rom_(rom)
{
}

uint8_t Bus::Read(uint16_t addr) const
{
  if(addr >= 0x0000 && addr <= 0x07FF) {
    return ram_.Read(addr);
  }
  else if(addr >= 0x2000 && addr <= 0x2007) {
    // PPU
    return ram_.Read(addr);
  }
  else if(addr >= 0x4000 && addr <= 0x401F) {
    // APU
    return ram_.Read(addr);
  }
  else if(addr >= 0x8000 && addr <= 0x8000+rom_.chrromStart_-1) {
    return rom_.ReadPrgRom(addr-0x8000);
  }
  else {
    throw std::runtime_error("Out of ROM!");
  }
}

void Bus::Write(uint16_t addr, uint8_t data)
{
  if(addr >= 0x0000 && addr <= 0x07FF) {
    ram_.Write(addr, data);
  }
  else if(addr >= 0x2000 && addr <= 0x2007) {
    // PPU
    ram_.Write(addr, data);
  }
  else if(addr >= 0x4000 && addr <= 0x401F) {
    // APU
    ram_.Write(addr, data);
  }
  else {
    throw std::runtime_error("Out of RAM!");
  }
}

void Bus::SetROM(std::string romfile)
{
  // rom_.Set(romfile);
}
