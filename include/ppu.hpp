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

struct PpuInternalRegister {
  uint16_t v;
  uint16_t t;
  uint8_t x;
  uint8_t w;
};

class Ppu {
private:
  PpuBus& ppubus_;
  Ram& palletram_;
  PpuInternalRegister internalRegs_{};
  uint16_t hi_{}, lo_{}, addr_{};
  uint16_t BaseNTAddr();
  uint8_t IncPpuAddrSize();
  uint16_t SpritePTAddr();
  uint16_t BackgroundPTAddr();
  uint8_t SpriteSize();
  bool VblankNMI();
public:
  PpuRegister regs{};
  Ppu(PpuBus& ppubus, Ram& palletram);
  void Tick();
  void WritePpuAddr();
  void ReadPpuData();
  void WritePpuData();
};
