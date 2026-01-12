#include "memory.hpp"

Memory::Memory(std::vector<uint8_t> data)
  : memory_(data)
{
}

uint16_t Memory::Size() const
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
  std::cout << "    | ";
  for(int i=0; i<0x20; i++) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
		<< i << " ";
  }
  std::cout << std::endl;
  
  for(int i=0; i<0x22; i++) {
    std::cout << "---";
  }
  std::cout << std::endl;
  
  for (int i=0; i<memory_.size()/0x20; i++) {
    std::cout << std::hex << std::setw(4) << std::setfill('0')
	      << i*0x20
	      << "| ";
    for(int j=0; j<0x20; j++) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
		<< static_cast<unsigned int>(memory_.at(i*0x20+j))
		<< " ";
    }
    std::cout << std::endl;
  }
}
