#pragma once

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

#include "memory.hpp"

using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Rom {
private:
  Memory memory_;
  std::vector<uint8_t> Load(std::string file);
  
public:
  uint32_t chrromStart_;
  Rom(std::string romfile);
  uint8_t ReadPrgRom(uint16_t addr) const;
  uint8_t ReadChrRom(uint16_t addr) const;
  void Dump() const;
};
