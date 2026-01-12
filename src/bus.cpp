#include "bus.hpp"

CpuBus::CpuBus(Ppu& ppu, Ram& wram, Rom& rom)
  : ppu_(ppu), wram_(wram), rom_(rom)
{
}

uint8_t CpuBus::Read(uint16_t addr) const
{
  /* WRAM and Mirrors */
  if(addr >= 0x0000 && addr <= 0x1FFF) {
    return wram_.Read(addr&0x07FF);
  }
  /* PPU registers and Mirrors */
  else if(addr >= 0x2000 && addr <= 0x3FFF) {
    uint8_t idx = addr&0x0007;
    switch(idx) {
    case 0x0:
      return ppu_.regs.ppuCtrl;
    case 0x1:
      return ppu_.regs.ppuMask;
    case 0x2:
      return ppu_.regs.ppuStatus;
    case 0x3:
      return ppu_.regs.oamAddr;
    case 0x4:
      return ppu_.regs.oamData;
    case 0x5:
      return ppu_.regs.ppuScroll;
    case 0x6:
      return ppu_.regs.ppuAddr;
    case 0x7:
      return ppu_.regs.ppuData;
    default:
      throw std::runtime_error("Out of WRAM.");
    }
  }
  /* APU registers */
  else if(addr >= 0x4000 && addr <= 0x401F) {
    uint8_t idx = addr&0x001F;
    switch(idx) {
    case 0x14:
      return ppu_.regs.oamDma;
    default:
      throw std::runtime_error("Out of WRAM.");
    }
  }
  /* PRG_ROM */
  else if(addr >= 0x8000 && addr <= 0xFFFF) {
    return rom_.ReadPrgRom(addr&0x7FFF);
  }
  else {
    throw std::runtime_error("Out of WRAM.");
  }
}

void CpuBus::Write(uint16_t addr, uint8_t data)
{
  /* WRAM and Mirrors */
  if(addr >= 0x0000 && addr <= 0x1FFF) {
    wram_.Write(addr&0x07FF, data);
  }
  /* PPU registers and Mirrors */
  else if(addr >= 0x2000 && addr <= 0x3FFF) {
    wram_.Write(addr&0x0007, data);
  }
  /* APU registers */
  else if(addr >= 0x4000 && addr <= 0x401F) {
    wram_.Write(addr, data);
  }
  else {
    throw std::runtime_error("Out of WRAM.");
  }
}
