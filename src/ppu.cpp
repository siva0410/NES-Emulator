#include "ppu.hpp"
#include "display.hpp"

Ppu::Ppu(PpuBus& ppubus, Ram& palletram, Display& display)
  : ppubus_(ppubus), palletram_(palletram), display_(display)
{
  for(uint32_t i=0; i<64; i++) {
    oam_.Write(i*4, 0xF0);
  }
}

uint8_t Ppu::GetBGPallet(Point p)
{
  uint8_t palletByte = ppubus_.Read(0x23C0+p.x/4+p.y/4*8);
  uint8_t palletIdx = (p.x/2)&0b01 | (((p.y/2)<<1)&0b10)<<1;
  uint8_t pallet = (palletByte >> palletIdx) & 0b11;
  return pallet;
}

RGB Ppu::GetSprColor(uint8_t palletIdx, uint8_t patternIdx)
{
  uint8_t colorIdx = ppubus_.Read(0x3F10+palletIdx*4+patternIdx);
  RGB color = pallet_.at(colorIdx);
  return color;
}

RGB Ppu::GetBGColor(uint8_t palletIdx, uint8_t patternIdx)
{
  uint8_t colorIdx = ppubus_.Read(0x3F00+palletIdx*4+patternIdx);
  RGB color = pallet_.at(colorIdx);
  return color;
}

void Ppu::DrawSprPattern(Point p, uint8_t attr, uint16_t chrIdx)
{
  uint8_t palletIdx = attr & 0b11;
  bool flipHorizon = attr>>6 & 0b1;
  bool flipVertical = attr>>7 & 0b1;
  uint16_t patternAddr = SpritePTAddr() + 0x10*chrIdx;
  uint32_t wx{};
  uint32_t wy{};
  
  for(uint32_t y=0; y<8; y++) {
    if(flipVertical){
       wy = 7-y;
    }
    else {
      wy = y;
    }
    uint8_t patternLow = ppubus_.Read(patternAddr+y);
    uint8_t patternHi = ppubus_.Read(patternAddr+y+0x8);
    for(uint32_t x=0; x<8; x++) {
      if(flipHorizon){
	wx = 7-x;
      }
      else {
	wx = x;
      }
      uint8_t patternIdx = patternLow>>(7-x) & 0x1 | (patternHi>>(7-x) & 0x1) << 1;
      Point patternPoint{p.x+wx, p.y+wy};
      display_.Write(patternPoint, GetSprColor(palletIdx, patternIdx));
    }
  }
}

void Ppu::DrawSprPatterns()
{
  uint8_t index{};
  uint8_t attr{};
  Point p{};
  for(uint32_t i=0; i<64; i++) {
    p.y = oam_.Read(i*4);
    index = oam_.Read(i*4+1);
    attr = oam_.Read(i*4+2);
    p.x = oam_.Read(i*4+3);
    if((attr>>5 & 0b1) == 0){
      DrawSprPattern(p, attr, index);
    }
  }
}

void Ppu::DrawBGPattern(Point p)
{
  uint8_t chrIdx = ppubus_.Read(0x2000+p.x+32*p.y);
  uint16_t patternAddr = BackgroundPTAddr() + 0x10*chrIdx;
  for(uint32_t y=0; y<8; y++) {
    uint8_t patternLow = ppubus_.Read(patternAddr+y);
    uint8_t patternHi = ppubus_.Read(patternAddr+y+0x8);
    for(uint32_t x=0; x<8; x++) {
      Point patternPoint{p.x*8+x, p.y*8+y};
      uint8_t patternIdx = patternLow>>(7-x) & 0x1 | (patternHi>>(7-x) & 0x1) << 1;
      display_.Write(patternPoint, GetBGColor(GetBGPallet(p), patternIdx));
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
    SetVblank();
    nmi_ = true;
    for(uint32_t x=0; x<32; x++) {
      uint32_t y = lines_/8-1;
      Point p{x,y};
      DrawBGPattern(p);
    }
  }
  if (lines_ >= 262) {
    DrawSprPatterns();
    // oam_.Dump();
    display_.Update();
    lines_ = 0;
    ClearVblank();
  }
}

bool Ppu::CheckNmi()
{
  if(nmi_) {
    nmi_ = false;
    return true;
  }
  return false;
}

void Ppu::WritePpuAddr(uint8_t data)
{
  regs.ppuAddr = data;
  if(!internalRegs_.w) {
    internalRegs_.v = regs.ppuAddr<<8;
    internalRegs_.w = 1;
  } else {
    internalRegs_.v |= regs.ppuAddr;
    internalRegs_.w = 0;
  }
}

uint8_t Ppu::ReadPpuData()
{
  regs.ppuData = ppubus_.Read(internalRegs_.v);
  internalRegs_.v += IncPpuAddrSize();
  return regs.ppuData;
}

void Ppu::WritePpuData(uint8_t data)
{
  regs.ppuData = data;
  if(internalRegs_.w) {
    throw std::runtime_error("PPU Error.");
  }
  ppubus_.Write(internalRegs_.v, regs.ppuData);
  internalRegs_.v += IncPpuAddrSize();
}

void Ppu::WriteOamAddr(uint8_t data)
{
  regs.oamAddr = data;
  internalRegs_.oam = regs.oamAddr;
}

uint8_t Ppu::ReadOamData()
{
  regs.oamData = oam_.Read(internalRegs_.oam);
  return regs.oamData;
}

void Ppu::WriteOamData(uint8_t data)
{
  regs.oamData = data;
  oam_.Write(internalRegs_.oam++, regs.oamData);
}

void Ppu::WriteOam(uint16_t addr, uint8_t data)
{
  oam_.Write(addr, data);
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
    return 0x1000;
  } else {
    return 0x0000;
  }
}

uint16_t Ppu::BackgroundPTAddr()
{
  if(regs.ppuCtrl>>4 & 0b1) {
    return 0x1000;
  } else {
    return 0x0000;
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

bool Ppu::SpriteOverflow() {
  return regs.ppuStatus>>5 & 0b1;
}

bool Ppu::Sprite0Hit() {
  return regs.ppuStatus>>6 & 0b1;
}

void Ppu::SetVblank() {
  regs.ppuStatus = regs.ppuStatus | 0b10000000;
}

void Ppu::ClearVblank() {
  regs.ppuStatus = regs.ppuStatus & 0b01111111;
}

bool Ppu::Vblank() {
  return regs.ppuStatus>>7 & 0b1;
}

uint8_t Ppu::ReadPpuStatus() {
  uint8_t status = regs.ppuStatus;
  ClearVblank();
  internalRegs_.w = 0;
  return status;
}
