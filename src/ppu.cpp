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
    CheckSprite0Hit(patternIdx);
    display_.Write(screen_, GetBGColor(GetBGPallet(), patternIdx));
  }
}

RGB Ppu::GetSprColor(uint8_t palletIdx, uint8_t patternIdx)
{
  uint8_t colorIdx = ppubus_.Read(0x3F10+palletIdx*4+patternIdx);
  RGB color = pallet_.at(colorIdx);
  return color;
}

uint8_t Ppu::GetSprPattern(Point screen, Point spr, uint8_t attr, uint16_t chrIdx)
{
  bool flipHorizon = attr>>6 & 0b1;
  bool flipVertical = attr>>7 & 0b1;
  uint16_t patternAddr = SpritePTAddr() + 0x10*chrIdx;
  Point pattern{};
  Point fine{};
  
  fine.x = screen.x - spr.x;
  fine.y = screen.y - spr.y;
  
  if(flipVertical){
    pattern.y = 7-fine.y;
  }
  else {
    pattern.y = fine.y;
  }
  
  uint8_t patternLow = ppubus_.Read(patternAddr+pattern.y);
  uint8_t patternHi = ppubus_.Read(patternAddr+pattern.y+0x8);

  if(flipHorizon){
    pattern.x = 7-fine.x;
  }
  else {
    pattern.x = fine.x;
  }
  uint8_t patternIdx = patternLow>>(7-pattern.x) & 0x1 | (patternHi>>(7-pattern.x) & 0x1) << 1;

  return patternIdx;
}

void Ppu::DrawSprPattern(Point spr, uint8_t attr, uint16_t chrIdx)
{
  uint8_t palletIdx = attr & 0b11;
  uint16_t patternAddr = SpritePTAddr() + 0x10*chrIdx;
  Point fine{};
  
  for(fine.y=0; fine.y<8; fine.y++) {
    for(fine.x=0; fine.x<8; fine.x++) {
      uint8_t patternIdx = GetSprPattern(Point{spr.x+fine.x,spr.y+fine.y}, spr, attr, chrIdx);
      if (patternIdx == 0) {
	continue;
      }
      Point patternPoint{spr.x+fine.x, spr.y+fine.y};
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
  Point spr{};
  for(uint32_t i=0; i<64; i++) {
    spr.y = oam_.Read(i*4);
    index = oam_.Read(i*4+1);
    attr = oam_.Read(i*4+2);
    spr.x = oam_.Read(i*4+3);
    if((attr>>5 & 0b1) == 0){
      if (showLeftSpr_ || spr.x >= 8) {
	DrawSprPattern(spr, attr, index);
      }
    }
  }
}

void Ppu::CheckSprite0Hit(uint8_t bgPatternIdx)
{
  if(Sprite0Hit()) return;
  if(!enableBg_) return;
  if(!enableSpr_) return;
  
  Point spr{};
  uint8_t index{};
  uint8_t attr{};
  spr.y = oam_.Read(0);
  index = oam_.Read(1);
  attr = oam_.Read(2);
  spr.x = oam_.Read(3);
  
  if(spr.x >= screen_.x || spr.x + 8 < screen_.x){
    return;
  }
  if(spr.y >= screen_.y || spr.y + 8 < screen_.y){
    return;
  }
  if((attr>>5 & 0b1) == 1){
    return;
  }
  
  uint8_t chrPatternIdx = GetSprPattern(screen_, spr, attr, index);
  if (bgPatternIdx != 0 && chrPatternIdx != 0) {
    SetSprite0Hit();
    std::cout << "Sprite0Hit! x: " << screen_.x << " y: " << screen_.y << std::endl;
    std::cout << "0 sprite x: " <<  spr.x << " y: " << spr.y << std::endl;
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
    if (lines_ == 262) {
      lines_ = 0;
      if (enableSpr_){
	DrawSprPatterns();
      }
      nmi_ = false;
      ClearVblank();
      ClearSprite0Hit();
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

void Ppu::SetSprite0Hit() {
  ppuStatus_ |= 0b01000000;
}

void Ppu::ClearSprite0Hit() {
  ppuStatus_ &= 0b10111111;
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
