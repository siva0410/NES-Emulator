#include "ram.hpp"

uint8_t Ram::Read(uint16_t addr) const
{
  return ram.at(addr);
}

void Ram::Write(uint16_t addr, uint8_t data)
{
  ram.at(addr) = data;
}
