#include "cpubus.hpp"

CpuBus::CpuBus(Ppu& ppu, Ram& wram, Controller& controller1, Controller& controller2)
  : ppu_(ppu), wram_(wram), controller1_(controller1), controller2_(controller2)
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
    case 0x2:
      return ppu_.ReadPpuStatus();
    case 0x4:
      return ppu_.ReadOamData();
    case 0x7:
      return ppu_.ReadPpuData();
    default:
      return 0x00;
      // throw std::runtime_error("READ: Out of ppu register.");
    }
  }
  /* APU or I/O registers */
  else if(addr >= 0x4000 && addr <= 0x401F) {
    uint8_t idx = addr&0x001F;
    switch(idx) {
    case 0x16:
      return controller1_.Read();
    case 0x17:
      return controller2_.Read();
    default:
      return 0x00;
      // throw std::runtime_error("READ: Out of apu or i/o register.");
    }
  }
  /* PRG_ROM */
  else if(addr >= 0x6000 && addr <= 0x7FFF) {
    return rom_->ReadRam(addr&0x1FFF);
  }
  else if(addr >= 0x8000 && addr <= 0xFFFF) {
    if (rom_->prgromSize == 0x4000) {
      return rom_->ReadPrgRom(addr&0x3FFF);
    }
    else {
      return rom_->ReadPrgRom(addr&0x7FFF);
    }
  }
  else {
    throw std::runtime_error("READ: Out of CPUBUS.");
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
      ppu_.WritePpuCtrl(data);
      break;
    case 0x1:
      ppu_.WritePpuMask(data);
      break;
    case 0x3:
      ppu_.WriteOamAddr(data);
      break;
    case 0x4:
      ppu_.WriteOamData(data);
      break;
    case 0x5:
      ppu_.WritePpuScroll(data);
      break;
    case 0x6:
      ppu_.WritePpuAddr(data);
      break;
    case 0x7:
      ppu_.WritePpuData(data);
      break;
    default:
      throw std::runtime_error("WRITE: Out of ppu register.");
    }
  }
  /* APU or I/O registers */
  else if(addr >= 0x4000 && addr <= 0x401F) {
    uint8_t idx = addr&0x001F;
    switch(idx) {
    case 0x14:
      Dma(data);
      break;
    case 0x16:
      controller1_.Write(data);
      break;
    case 0x17:
      controller2_.Write(data);
      break;
    default:
      break;
      throw std::runtime_error("WRITE: Out of apu or i/o registers.");
    }
  }
  else if(addr >= 0x6000 && addr <= 0x7FFF) {
    rom_->WriteRam(addr&0x1FFF, data);
  }
  else {
    throw std::runtime_error("WRITE: Out of CPUBUS.");
  }
}

void CpuBus::Dma(uint8_t data)
{
  uint16_t dmaAddr = data << 8;
  for (uint32_t oamAddr=0; oamAddr<0x100; oamAddr++) {
    ppu_.WriteOam(oamAddr, wram_.Read(dmaAddr+oamAddr));
  }
  dma_ = true;
}

bool CpuBus::ConsumeDma()
{
  if(dma_) {
    dma_ = false;
    return true;
  }
  return false;
}
