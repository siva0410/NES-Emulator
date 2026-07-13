#include "ppu.hpp"
#include "display.hpp"
#include <iterator>

Ppu::Ppu(PpuBus& ppubus, Ram& palletram, Display& display)
  : ppubus_(ppubus), display_(display)
{
  for(uint32_t i=0; i<64; i++) {
    oam_.Write(i*4, 0xF0);
  }
}

uint8_t Ppu::GetBGPallet(uint8_t ntIdx)
{
  uint8_t palletByte = ppubus_.Read(0x2000+ntIdx*0x400+0x3C0+(coarse_.x>>2)+(coarse_.y>>2)*8);
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

void Ppu::DrawBGPixel(Point screen)
{
  coarse_.x = v_&0x1F;
  coarse_.y = (v_>>5)&0x1F;
  fine_.x = (nextFinex_ + screen.x)&0x7;
  fine_.y = (v_>>12)&0x7;
  uint8_t ntIdx = (v_>>10)&0b11;
  uint8_t chrIdx = ppubus_.Read(0x2000+ntIdx*0x400+coarse_.x+coarse_.y*32);
  uint16_t patternAddr = BackgroundPTAddr() + 0x10*chrIdx;
  uint8_t patternLow = ppubus_.Read(patternAddr+fine_.y);
  uint8_t patternHi = ppubus_.Read(patternAddr+0x8+fine_.y);
  uint8_t patternIdx = (patternLow>>(7-fine_.x) & 0b1) | ((patternHi>>(7-fine_.x) & 0b1) << 1);
  if (patternIdx != 0) {
    existPattern_.at(screen.x + screen.y*256) = true;
  }
  else {
    existPattern_.at(screen.x + screen.y*256) = false;
  }
  CheckSprite0Hit(screen, patternIdx);
  display_.Write(screen, GetBGColor(GetBGPallet(ntIdx), patternIdx));
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
  Point fine{screen.x - spr.x, screen.y - spr.y};
  Point pattern{};
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
      
      bool behindBG = attr>>5 & 0b1;
      if (behindBG && existPattern_.at(patternPoint.x+patternPoint.y*256)) {
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
  for(int32_t i=63; i>=0; --i) {
    spr.y = oam_.Read(i*4) + 1;
    index = oam_.Read(i*4+1);
    attr = oam_.Read(i*4+2);
    spr.x = oam_.Read(i*4+3);
    
    if (showLeftSpr_ || spr.x >= 8) {
      DrawSprPattern(spr, attr, index);
    }
  }
}

void Ppu::CheckSprite0Hit(Point screen, uint8_t bgPatternIdx)
{
  if(Sprite0Hit()) return;
  if(!enableBg_) return;
  if(!enableSpr_) return;
  if (screen.x < 8 && (!showLeftBg_ || !showLeftSpr_)) return;
  if (screen.x == 255) return;
  
  Point spr{};
  uint8_t index{};
  uint8_t attr{};
  spr.y = oam_.Read(0) + 1;
  index = oam_.Read(1);
  attr = oam_.Read(2);
  spr.x = oam_.Read(3);

  if(spr.x > screen.x || spr.x + 8 <= screen.x){
    return;
  }
  if(spr.y > screen.y || spr.y + 8 <= screen.y){
    return;
  }

  uint8_t chrPatternIdx = GetSprPattern(screen, spr, attr, index);
  if (bgPatternIdx != 0 && chrPatternIdx != 0) {
    SetSprite0Hit();
  }
}

void Ppu::Clock()
{
  bool rendering = lines_ < 240 || lines_ == 261;
  
  if(cycles_ < 256 && lines_ < 240) {
    Point screen{cycles_, lines_};
    if (enableBg_) {
      if (showLeftBg_ || screen.x >= 8) {
	DrawBGPixel(screen);
      }
    }
    if (enableBg_ || enableSpr_) {
      if (((screen.x + nextFinex_) & 0x7) == 7){
	// increment x
	if ((v_ & 0x1F) == 31) {
	  // toggle nametable x
	  v_ ^= 0x0400;
	  // reset couarse x
	  v_ &= ~0x001F;
	} else {
	  v_++;
	}
      }
    }
  }
  
  if(cycles_ == 256 && rendering) {
    if (enableBg_ || enableSpr_) {
      if ((v_&0x7000) != 0x7000) {
	// increment fine y
	v_ += 0x1000;
      }
      else {
	uint32_t coarsey = (v_>>5)&0x1F;
	// reset fine y
	v_ &= ~0x7000;
	if (coarsey == 29) {
	  // toggle nametable y
	  v_ ^= 0x0800;
	  coarsey = 0;
	}
	else if (coarsey == 31) {
	  coarsey = 0;
	}
	else {
	  coarsey++;
	}
	v_ = (v_ & ~0x03E0) | (coarsey << 5);
      }
    }
  }
  
  if(cycles_ == 257 && rendering) {
    if (enableBg_ || enableSpr_) {
      // coarse x
      v_ = (v_&~0x001F) | (t_&0x001F);
      // nametable x
      v_ = (v_&~0x0400) | (t_&0x0400);

      nextFinex_ = finex_;
    }
  }

  if (lines_ == 240 && cycles_ == 0) {
    if (enableSpr_){
      DrawSprPatterns();
    }
    display_.Update();
    existPattern_.fill(false);
  }
  
  if (lines_ == 241 && cycles_ == 1) {
    SetVblank();
    nmi_ = true;
  }

  if (lines_ == 261 && cycles_ == 1) {
    ClearVblank();
    ClearSprite0Hit();
  }
  
  if(lines_ == 261 && cycles_ >= 280 && cycles_ <= 304) {
    if (enableBg_ || enableSpr_) {
      // fine y
      v_ = (v_&~0x7000) | (t_&0x7000);
      // coarse y
      v_ = (v_&~0x03E0) | (t_&0x03E0);
      // nametable y
      v_ = (v_&~0x0800) | (t_&0x0800);
    }
  }

  if (cycles_ == 340) {
    lines_++;
    cycles_ = 0;
    if (lines_ == 262) {
      lines_ = 0;
      nmi_ = false;
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
  t_ = (t_&~0x0C00) | ((data&0b11)<<10);
}

void Ppu::WritePpuMask(uint8_t data)
{
  showLeftBg_ = (data >> 1) & 0b1;
  showLeftSpr_ = (data >> 2) & 0b1;
  enableBg_ = (data >> 3) & 0b1;
  enableSpr_ = (data >> 4) & 0b1;
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
    t_ = t_ & 0x7FE0;
    t_ |= data >> 3;
    finex_ = data & 0b111;
    latch_ = true;
  } else {
    t_ = t_ & 0x0C1F;
    t_ |= (data >> 3)<<5; 
    t_ |= (data & 0b111)<<12;
    latch_ = false;
  }
}

void Ppu::WritePpuAddr(uint8_t data)
{
  if(!latch_) {
    t_ = data<<8;
    t_ &= 0x3FFF;
    latch_ = true;
  } else {
    t_ |= data;
    t_ &= 0x3FFF;
    v_ = t_;
    latch_ = false;
  }
}

uint8_t Ppu::ReadPpuData()
{
  uint8_t result{};
  uint16_t addr = v_&0x3FFF;
  uint8_t data = ppubus_.Read(v_);
  if (addr < 0x3F00) {
    result = oldPpuData_;
    oldPpuData_ = data;
  }
  else {
    result = data;
    oldPpuData_ = ppubus_.Read(addr - 0x1000);
  }
  v_ += IncPpuAddrSize();
  v_ &= 0x3FFF;
  return result;
}

void Ppu::WritePpuData(uint8_t data)
{
  if(latch_) {
    throw std::runtime_error("PPU Latch Error.");
  }
  ppubus_.Write(v_, data);
  v_ += IncPpuAddrSize();
  v_ &= 0x3FFF;
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
