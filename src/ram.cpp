#include "ram.hpp"

unsigned char Ram::Read(unsigned int addr) const
{
  return ram.at(addr);
}

void Ram::Write(unsigned int addr, unsigned char data)
{
  ram.at(addr) = data;
}
