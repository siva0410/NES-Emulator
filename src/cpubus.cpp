#include "cpubus.hpp"

CpuBus::CpuBus(Ppu& ppu, Ram& wram)
  : ppu_(ppu), wram_(wram)
{
}

void CpuBus::SetRom(Rom* rom)
{
  rom_ = rom;
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
    // case 0x0:
    //   return ppu_.regs.ppuCtrl;
    // case 0x1:
    //   return ppu_.regs.ppuMask;
    case 0x2:      
      return ppu_.ReadPpuStatus();
    // case 0x3:
    //   return ppu_.regs.oamAddr;
    case 0x4:
      return ppu_.ReadOamData();
    // case 0x5:
    //   return ppu_.regs.ppuScroll;
    // case 0x6:
    //   return ppu_.regs.ppuAddr;
    case 0x7:
      return ppu_.ReadPpuData();
    default:
      throw std::runtime_error("Out of ppu register.");
    }
  }
  /* APU or I/O registers */
  else if(addr >= 0x4000 && addr <= 0x401F) {
    uint8_t idx = addr&0x001F;
    switch(idx) {
    case 0x14:
      return ppu_.regs.oamDma;
    default:
      throw std::runtime_error("Out of apu or i/o register.");
    }
  }
  /* PRG_ROM */
  else if(addr >= 0x8000 && addr <= 0xFFFF) {
    return rom_->ReadPrgRom(addr&0x7FFF);
  }
  else {
    throw std::runtime_error("Out of PRG_ROM.");
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
    uint8_t idx = addr&0x0007;
    switch(idx) {
    case 0x0:
      ppu_.regs.ppuCtrl = data;
      break;
    case 0x1:
      ppu_.regs.ppuMask = data;
      break;
    // case 0x2:
    //   ppu_.regs.ppuStatus = data;
    //   break;
    case 0x3:
      ppu_.WriteOamAddr(data);
      break;
    case 0x4:
      ppu_.WriteOamData(data);
      break;
    case 0x5:
      ppu_.regs.ppuScroll = data;
      break;
    case 0x6:
      ppu_.WritePpuAddr(data);
      break;
    case 0x7:
      ppu_.WritePpuData(data);
      break;
    default:
      throw std::runtime_error("Out of ppu register.");
    }
  }
  /* APU or I/O registers */
  else if(addr >= 0x4000 && addr <= 0x401F) {
    uint8_t idx = addr&0x001F;
    switch(idx) {
    case 0x14:
      ppu_.regs.oamDma = data;
    default:
      throw std::runtime_error("Out of apu or i/o registers.");
    }
  }
  else {
    throw std::runtime_error("Out of WRAM.");
  }
}
