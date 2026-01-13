#pragma once

#include <cstdint>
#include <string>

#include "ram.hpp"
#include "rom.hpp"
#include "ppu.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class CpuBus {
private:
  Rom* rom_{};
  Ram& wram_;
  Ppu& ppu_;
public:
  CpuBus(Ppu& ppu, Ram& wram);
  void SetRom(Rom* rom);
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
};
