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

  uint32_t prgromSize = data.at(4)*0x4000;
  uint32_t chrromSize = data.at(5)*0x2000;
  chrromStart_ = 0x10+prgromSize;
  
  return data;
}

uint8_t Rom::ReadPrgRom(uint16_t addr) const
{
  return memory_.Read(addr+0x10);
}

uint8_t Rom::ReadChrRom(uint16_t addr) const
{
  return memory_.Read(addr+chrromStart_);
}

void Rom::Dump() const
{
  memory_.Dump();
}
