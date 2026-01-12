#pragma once

#include <cstdint>
#include <string>

#include "ppubus.hpp"
#include "memory.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

struct PpuRegister {
  uint8_t ppuCtrl;
  uint8_t ppuMask;
  uint8_t ppuStatus;
  uint8_t oamAddr;
  uint8_t oamData;
  uint8_t ppuScroll;
  uint8_t ppuAddr;
  uint8_t ppuData;
  uint8_t oamDma;
};

class Ppu {
private:
  PpuBus& ppubus_;
  Ram& palletram_;
public:
  PpuRegister regs;
  Ppu(PpuBus& ppubus, Ram& palletram);
  uint8_t ReadPallet(uint16_t addr) const;
  void WritePallet(uint16_t addr, uint8_t data);
};
