#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

#include "memory.hpp"

using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Ram {
private:
  Memory memory_;
  
public:
  Ram(size_t size);
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
};
