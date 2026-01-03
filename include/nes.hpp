#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

#include "ram.hpp"
#include "rom.hpp"
#include "bus.hpp"
#include "cpu.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Nes {
private:
  Ram ram{};
  Rom rom{};
  Bus bus{&ram, &rom};
  Cpu cpu{&bus};
public:
  void SetROM(std::string romfile);
  void Start();
};
