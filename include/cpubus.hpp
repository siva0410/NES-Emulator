#pragma once

#include <cstdint>

#include "ram.hpp"
#include "rom.hpp"
#include "ppu.hpp"
#include "controller.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class CpuBus {
private:
  bool dma_{};
  Rom* rom_{};
  Ram& wram_;
  Ppu& ppu_;
  Controller& controller1_;
  Controller& controller2_;
public:
  CpuBus(Ppu& ppu, Ram& wram, Controller& controller1, Controller& controller2);
  void SetRom(Rom* rom);
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
  void Dma(uint8_t data);
  bool ConsumeDma();
};
