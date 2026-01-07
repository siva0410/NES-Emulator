#include "memory.hpp"

Memory::Memory()
  : Memory(std::vector<uint8_t>(0))
{
}

Memory::Memory(std::vector<uint8_t> data)
  : memory_(data)
{
}

size_t Memory::Size() const
{
  std::cout << memory_.size() << std::endl;
  return memory_.size();
}

uint8_t* Memory::Data()
{
  return memory_.data();
}

uint8_t Memory::Read(uint16_t addr) const
{
  return memory_.at(addr);
}

void Memory::Write(uint16_t addr, uint8_t data)
{
  memory_.at(addr) = data;
}

void Memory::Dump() const
{
  for (int i=0; i<memory_.size(); i++) {
    if(i%0x20==0) std::cout << std::endl;
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << memory_.at(i)
              << ' ';
  }
  std::cout << std::endl;
}
