#pragma once

#include <array>
#include <cstdint>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Ram {
private:
  std::array<uint8_t, 0x10000> ram{};
public:
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
};
