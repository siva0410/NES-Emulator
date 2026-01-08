#include "rom.hpp"

Rom::Rom(std::string romfile)
  : memory_(Load(romfile))
{
}

std::vector<uint8_t> Rom::Load(std::string romfile)
{
  auto size = std::filesystem::file_size(romfile);
  std::vector<uint8_t> data(size);
  
  std::ifstream ifs{romfile, std::ios::binary};
  ifs.read(reinterpret_cast<char*>(data.data()), data.size());

  return data;
}

uint8_t Rom::Read(uint16_t addr) const
{
  return memory_.Read(addr);
}

void Rom::Dump() const
{
  memory_.Dump();
}
