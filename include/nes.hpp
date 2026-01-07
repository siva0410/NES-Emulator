#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cstddef>

#include "ram.hpp"
#include "rom.hpp"
#include "bus.hpp"
#include "cpu.hpp"

using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Nes {
private:
  Rom rom_;
  Ram ram_{0x8000};
  Bus bus_{ram_, rom_};
  Cpu cpu_{bus_};
public:
  Nes(std::string romfile);
  void SetROM(std::string romfile);
  void Start();
};
