#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdint>

#include "memory.hpp"
#include "ram.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Rom {
private:
  uint32_t chrromStart_;
  uint8_t mirroring_{};
  Memory rom_;
  Ram prgram_{0x2000};
  Ram chrram_{0x2000};
  
  std::vector<uint8_t> Load(std::string file);
public:
  uint32_t prgromSize;
  uint32_t chrromSize;
  Rom(std::string romfile);
  bool IsVerticalMirror() const;
  uint8_t ReadPrgRom(uint16_t addr) const;
  uint8_t ReadPrgRam(uint16_t addr) const;
  void WritePrgRam(uint16_t addr, uint8_t data);
  bool IsChrRam() const;
  uint8_t ReadChrRom(uint16_t addr) const;
  uint8_t ReadChrRam(uint16_t addr) const;
  void WriteChrRam(uint16_t addr, uint8_t data);
  void Dump() const;
  void RamDump() const;
};
