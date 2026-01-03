#pragma once

#include <cstdint>
#include <string>

#include "ram.hpp"
#include "rom.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Bus {
private:
  Ram* ram_;
  Rom* rom_;
public:
  Bus(Ram* ram, Rom* rom);
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
  void SetROM(std::string romfile);
};
