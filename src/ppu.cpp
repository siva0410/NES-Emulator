#include "ppu.hpp"
#include "display.hpp"

Ppu::Ppu(PpuBus& ppubus, Ram& palletram, Display& display)
  : ppubus_(ppubus), palletram_(palletram), display_(display)
{
}

void Ppu::Tick()
{
  for(uint32_t y=0; y<30; y++) {
    for(uint32_t x=0; x<32; x++) {
      // Attribute
      uint8_t attrByte = ppubus_.Read(0x23C0+x/4+y/4*8);
      uint8_t attrIdx = (x/2)&0b01 | (((y/2)<<1)&0b10)<<1;
      uint8_t attrNum = (attrByte >> attrIdx) & 0b11;
      uint16_t colorPalletAddr = 0x3F00+attrNum*4;
      // Sprite
      uint8_t spriteIdx = ppubus_.Read(0x2000+x+32*y);
      for(uint32_t spriteY=0; spriteY<8; spriteY++) {
	uint8_t spriteLow = ppubus_.Read(0x10*spriteIdx+spriteY);
	uint8_t spriteHi = ppubus_.Read(0x10*spriteIdx+spriteY+0x8);
	for(uint32_t spriteX=0; spriteX<8; spriteX++) {
	  uint8_t spriteNum = spriteLow>>(7-spriteX) & 0x1 | (spriteHi>>(7-spriteX) & 0x1) << 1;
	  Point p{x*8+spriteX, y*8+spriteY};
	  uint8_t colorIdx = ppubus_.Read(colorPalletAddr+spriteNum);
	  display_.Write(p, pallet_.at(colorIdx));
	}
      }
    }
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

