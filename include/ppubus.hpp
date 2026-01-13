#pragma once

#include <cstdint>
#include <string>

#include "ram.hpp"
#include "rom.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class PpuBus {
private:
  Rom* rom_{};
  Ram& vram_;
  Ram& palletram_;
public:
  PpuBus(Ram& vram, Ram& palletram);
  void SetRom(Rom* rom);
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
};
