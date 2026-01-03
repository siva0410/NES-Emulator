#include "rom.hpp"

void Rom::Set(std::string romfile)
{
  std::ifstream in{romfile, std::ios::binary};
  in.read(reinterpret_cast<char*>(rom.data()), static_cast<std::streamsize>(rom.size()));  
}

unsigned char Rom::Read(unsigned int addr) const
{
  return rom.at(addr);
}

