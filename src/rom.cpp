#include "rom.hpp"

Rom::Rom(std::string romfile)
  : rom_(Load(romfile))
{
  prgromSize = rom_.Read(4)*0x4000;
  chrromSize = rom_.Read(5)*0x2000;
  chrromStart_ = 0x10+prgromSize;
  mirroring_ = rom_.Read(6) & 0b1;
}

bool Rom::IsChrRam() const
{
  if(chrromSize == 0) {
    return true;
  }
  return false;
}

bool Rom::IsVerticalMirror() const
{
  if(mirroring_ == 1) {
    return true;
  }
  return false;
}

std::vector<uint8_t> Rom::Load(std::string romfile)
{
  auto size = std::filesystem::file_size(romfile);
  std::vector<uint8_t> data(size);
  
  std::ifstream ifs{romfile, std::ios::binary};
  ifs.read(reinterpret_cast<char*>(data.data()), data.size());
  
  return data;
}

uint8_t Rom::ReadPrgRom(uint16_t addr) const
{
  return rom_.Read(addr+0x10);
}

uint8_t Rom::ReadChrRom(uint16_t addr) const
{
  return rom_.Read(addr+chrromStart_);
}

uint8_t Rom::ReadPrgRam(uint16_t addr) const
{
  return prgram_.Read(addr);
}

void Rom::WritePrgRam(uint16_t addr, uint8_t data)
{
  prgram_.Write(addr, data);
}

uint8_t Rom::ReadChrRam(uint16_t addr) const
{
  return chrram_.Read(addr);
}

void Rom::WriteChrRam(uint16_t addr, uint8_t data)
{
  chrram_.Write(addr, data);
}

void Rom::Dump() const
{
  rom_.Dump();
}

void Rom::RamDump() const
{
  prgram_.Dump();
}
