#include "ram.hpp"

Ram::Ram(size_t size)
  : memory_(std::vector<uint8_t>(size))
{
}

uint8_t Ram::Read(uint16_t addr) const
{
  return memory_.Read(addr);
}

void Ram::Write(uint16_t addr, uint8_t data)
{
  memory_.Write(addr, data);
}
