#include "ppu.hpp"

Ppu::Ppu(PpuBus& ppubus, Ram& palletram)
  : ppubus_(ppubus), palletram_(palletram)
{
}

void Ppu::Tick()
{
  if(!internalRegs_.w) {
    internalRegs_.v = regs.ppuAddr<<8;
    internalRegs_.w = 1;
  } else {
    internalRegs_.v |= regs.ppuAddr;
    ppubus_.Write(internalRegs_.v, regs.ppuData);
    internalRegs_.v += IncPpuAddrSize();
    internalRegs_.w = 0;
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
