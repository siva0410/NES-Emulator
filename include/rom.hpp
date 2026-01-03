#pragma once

#include <fstream>
#include <array>
#include <cstdint>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Rom {
private:
  std::array<uint8_t, 0x8000> rom{};
public:
  void Set(std::string romfile);
  uint8_t Read(uint16_t addr) const;
};
