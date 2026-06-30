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

  prgromSize = data.at(4)*0x4000;
  chrromSize = data.at(5)*0x2000;
  chrromStart_ = 0x10+prgromSize;

  std::printf("NES header: %c%c%c %02X\n", data[0], data[1], data[2], data[3]);
  std::printf("PRG banks = %u\n", data[4]);
  std::printf("CHR banks = %u\n", data[5]);
  std::printf("Flags6    = %02X\n", data[6]);
  std::printf("Flags7    = %02X\n", data[7]);

  uint8_t mapper = (data[6] >> 4) | (data[7] & 0xF0);
  std::printf("Mapper    = %u\n", mapper);
  
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

uint8_t Rom::ReadRam(uint16_t addr) const
{
  return ram_.Read(addr);
}

void Rom::WriteRam(uint16_t addr, uint8_t data)
{
  ram_.Write(addr, data);
}

void Rom::Dump() const
{
  memory_.Dump();
}

void Rom::RamDump() const
{
  ram_.Dump();
}
