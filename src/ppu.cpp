#include "ppu.hpp"

Ppu::Ppu(PpuBus& ppubus, Ram& palletram)
  : ppubus_(ppubus), palletram_(palletram)
{
}

uint8_t Ppu::ReadPallet(uint16_t addr) const
{
  return palletram_.Read(addr);
}

void Ppu::WritePallet(uint16_t addr, uint8_t data)
{
  palletram_.Write(addr, data);
}
