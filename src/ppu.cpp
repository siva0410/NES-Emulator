#include "ppu.hpp"
#include "display.hpp"

Ppu::Ppu(PpuBus& ppubus, Ram& palletram, Display& display)
  : ppubus_(ppubus), palletram_(palletram), display_(display)
{
}

uint8_t Ppu::GetBGColorPallet(Point p, uint8_t patternNum)
{
  uint8_t attrByte = ppubus_.Read(0x23C0+p.x/4+p.y/4*8);
  uint8_t attrIdx = (p.x/2)&0b01 | (((p.y/2)<<1)&0b10)<<1;
  uint8_t attrNum = (attrByte >> attrIdx) & 0b11;
  uint8_t colorIdx = ppubus_.Read(0x3F00+attrNum*4+patternNum);
  return colorIdx;
}

void Ppu::DrawBGPattern(Point p)
{
  uint8_t chrIdx = ppubus_.Read(0x2000+p.x+32*p.y);
  for(uint32_t y=0; y<8; y++) {
    uint8_t patternLow = ppubus_.Read(0x10*chrIdx+y);
    uint8_t patternHi = ppubus_.Read(0x10*chrIdx+y+0x8);
    for(uint32_t x=0; x<8; x++) {
      Point patternPoint{p.x*8+x, p.y*8+y};
      uint8_t patternNum = patternLow>>(7-x) & 0x1 | (patternHi>>(7-x) & 0x1) << 1;
      display_.Write(patternPoint, pallet_.at(GetBGColorPallet(p, patternNum)));
    }
  }
}

void Ppu::Clock()
{
  cycles_++;
  if (cycles_ <= 340) {
    return;
  }
  cycles_ = 0;
  lines_++;
  
  if (lines_ <= 240 and lines_%8 == 0) {
    for(uint32_t x=0; x<32; x++) {
      uint32_t y = lines_/8-1;
      Point p{x,y};
      DrawBGPattern(p);
    }
  }
  if (lines_ >= 262) {
    display_.Update();
    lines_ = 0;
  }
}

void Ppu::WritePpuAddr()
{
  if(!internalRegs_.w) {
    internalRegs_.v = regs.ppuAddr<<8;
    internalRegs_.w = 1;
  } else {
    internalRegs_.v |= regs.ppuAddr;
    internalRegs_.w = 0;
  }
}

void Ppu::ReadPpuData()
{
  regs.ppuData = ppubus_.Read(internalRegs_.v);
  internalRegs_.v += IncPpuAddrSize();
}

void Ppu::WritePpuData()
{
  
  if(internalRegs_.w) {
    throw std::runtime_error("PPU Error.");
  }
  ppubus_.Write(internalRegs_.v, regs.ppuData);
  internalRegs_.v += IncPpuAddrSize();
}

uint16_t Ppu::BaseNTAddr()
{
  uint8_t num = regs.ppuCtrl>>0 & 0b11;
  switch(num) {
  case 0:
    return 0x2000;
  case 1:
    return 0x2400;
  case 2:
    return 0x2800;
  case 3:
    return 0x2c00;
  default:
    throw std::runtime_error("Out of range.");
  }
}

uint8_t Ppu::IncPpuAddrSize()
{
  if(regs.ppuCtrl>>2 & 0b1) {
    return 0x20;
  } else {
    return 0x01;
  }
}

uint16_t Ppu::SpritePTAddr()
{
  if(regs.ppuCtrl>>3 & 0b1) {
    return 0x0000;
  } else {
    return 0x1000;
  }
}

uint16_t Ppu::BackgroundPTAddr()
{
  if(regs.ppuCtrl>>4 & 0b1) {
    return 0x0000;
  } else {
    return 0x1000;
  }
}

uint8_t Ppu::SpriteSize()
{
  if(regs.ppuCtrl>>5 & 0b1) {
    return 0x00;
  } else {
    return 0x01;
  }
}

bool Ppu::VblankNMI() {
  return regs.ppuCtrl>>7 & 0b1;
}

