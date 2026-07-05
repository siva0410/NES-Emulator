#include "ppu.hpp"
#include "display.hpp"
#include <iterator>

Ppu::Ppu(PpuBus& ppubus, Ram& palletram, Display& display)
  : ppubus_(ppubus), palletram_(palletram), display_(display)
{
  for(uint32_t i=0; i<64; i++) {
    oam_.Write(i*4, 0xF0);
  }
}

uint8_t Ppu::GetBGPallet()
{
  uint8_t palletByte = ppubus_.Read(0x2000+ntIdx_*0x400+0x3C0+(coarse_.x>>2)+(coarse_.y>>2)*8);
  uint8_t palletIdx = (coarse_.x>>1)&0b1 | ((coarse_.y>>1)&0b1)<<1;
  palletIdx = palletIdx<<1;
  uint8_t pallet = (palletByte >> palletIdx) & 0b11;
  return pallet;
}

RGB Ppu::GetBGColor(uint8_t palletIdx, uint8_t patternIdx)
{
  uint16_t addr{};
  if (patternIdx == 0) {
    addr = 0x3F00;
  }
  else {
    addr = 0x3F00+palletIdx*4+patternIdx;
  }
  uint8_t colorIdx = ppubus_.Read(addr);
  RGB color = pallet_.at(colorIdx);
  return color;
}

void Ppu::DrawBGPixel()
{
  bg_.x = screen_.x + scroll_.x;
  bg_.y = screen_.y + scroll_.y;
  ntIdx_ = (ppuCtrl_&0b11) ^ (bg_.x/256)&0b1 ^ (((bg_.y/240)&0b1)<<1);
  coarse_.x = (bg_.x>>3) % 32;
  coarse_.y = (bg_.y>>3) % 30;
  fine_.x = bg_.x & 0b111;
  fine_.y = bg_.y & 0b111;
  
  uint8_t chrIdx = ppubus_.Read(0x2000+ntIdx_*0x400+coarse_.x+coarse_.y*32);
  uint16_t patternAddr = BackgroundPTAddr() + 0x10*chrIdx;
  uint8_t patternLow = ppubus_.Read(patternAddr+fine_.y);
  uint8_t patternHi = ppubus_.Read(patternAddr+0x8+fine_.y);
  uint8_t patternIdx = (patternLow>>(7-fine_.x) & 0b1) | ((patternHi>>(7-fine_.x) & 0b1) << 1);
  if (bg_.y < 240) {
    display_.Write(screen_, GetBGColor(GetBGPallet(), patternIdx));
  }
}

RGB Ppu::GetSprColor(uint8_t palletIdx, uint8_t patternIdx)
{
  uint8_t colorIdx = ppubus_.Read(0x3F10+palletIdx*4+patternIdx);
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
      if (patternIdx == 0) {
	continue;
      }
      Point patternPoint{p.x+wx, p.y+wy};
      if (patternPoint.x < 0 || patternPoint.x >= 256) {
	continue;
      }
      if (patternPoint.y < 0 || patternPoint.y >= 240) {
	continue;
      }
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
      if (showLeftSpr_ || p.x >= 8) {
	DrawSprPattern(p, attr, index);
      }
    }
  }
}

void Ppu::Clock()
{
  if (cycles_ < 256 && lines_ < 240) {
    screen_.x = cycles_;
    screen_.y = lines_;
    if (enableBg_) {
      if (showLeftBg_ || screen_.x >= 8) {
	DrawBGPixel();
      } 
    }
  }
  
  if (lines_ == 240 && cycles_ == 0) {
    SetVblank();
    nmi_ = true;
  }
  
  if (cycles_ == 340) {
    lines_++;
    cycles_ = 0;
    if (lines_ == 260) {
      lines_ = 0;
      if (enableSpr_){
	DrawSprPatterns();
      }
      ClearVblank();
      display_.Update();
    } 
  }
  else {
    cycles_++;
  }
}

bool Ppu::ConsumeNmi()
{
  if(nmi_) {
    nmi_ = false;
    return true;
  }
  return false;
}

uint8_t Ppu::IncPpuAddrSize()
{
  if(ppuCtrl_>>2 & 0b1) {
    return 0x20;
  } else {
    return 0x01;
  }
}

uint16_t Ppu::SpritePTAddr()
{
  if(ppuCtrl_>>3 & 0b1) {
    return 0x1000;
  } else {
    return 0x0000;
  }
}

uint16_t Ppu::BackgroundPTAddr()
{
  if(ppuCtrl_>>4 & 0b1) {
    return 0x1000;
  } else {
    return 0x0000;
  }
}

uint8_t Ppu::SpriteSize()
{
  if(ppuCtrl_>>5 & 0b1) {
    return 0x00;
  } else {
    return 0x01;
  }
}

bool Ppu::IsNmiEnable() {
  return ppuCtrl_>>7 & 0b1;
}

void Ppu::WritePpuCtrl(uint8_t data)
{
  ppuCtrl_ = data;
}

void Ppu::WritePpuMask(uint8_t data)
{
  showLeftBg_ = data >> 1;
  showLeftSpr_ = data >> 2;
  enableBg_ = data >> 3;
  enableSpr_ = data >> 4;
}

uint8_t Ppu::ReadPpuStatus() {
  uint8_t status = ppuStatus_;
  ClearVblank();
  latch_ = false;
  return status;
}

void Ppu::WritePpuScroll(uint8_t data)
{
  if(!latch_) {
    scroll_.x = data;
    latch_ = true;
  } else {
    scroll_.y = data;
    latch_ = false;
  }
}

void Ppu::WritePpuAddr(uint8_t data)
{
  if(!latch_) {
    ppuAddr_ = data<<8;
    latch_ = true;
  } else {
    ppuAddr_ |= data;
    latch_ = false;
  }
}

uint8_t Ppu::ReadPpuData()
{
  uint8_t data = ppubus_.Read(ppuAddr_);
  ppuAddr_ += IncPpuAddrSize();
  return data;
}

void Ppu::WritePpuData(uint8_t data)
{
  if(latch_) {
    throw std::runtime_error("PPU Latch Error.");
  }
  ppubus_.Write(ppuAddr_, data);
  ppuAddr_ += IncPpuAddrSize();
}

void Ppu::WriteOamAddr(uint8_t data)
{
  oamAddr_ = data;
}

uint8_t Ppu::ReadOamData()
{
  return oam_.Read(oamAddr_);
}

void Ppu::WriteOamData(uint8_t data)
{
  oam_.Write(oamAddr_++, data);
}

void Ppu::WriteOam(uint16_t addr, uint8_t data)
{
  oam_.Write(addr, data);
}

bool Ppu::SpriteOverflow() {
  return ppuStatus_>>5 & 0b1;
}

bool Ppu::Sprite0Hit() {
  return ppuStatus_>>6 & 0b1;
}

void Ppu::SetVblank() {
  ppuStatus_ |= 0b10000000;
}

void Ppu::ClearVblank() {
  ppuStatus_ &= 0b01111111;
}

bool Ppu::Vblank() {
  return ppuStatus_>>7 & 0b1;
}
