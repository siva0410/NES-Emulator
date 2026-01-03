#pragma once

#include <cstdint>

#include "bus.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Cpu {
private:
  Bus* bus_;
public:
  Cpu(Bus* bus);
  void Start();
};
