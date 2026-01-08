#include "cpu.hpp"

bool Cpu::Carry()
{
  return regs_.flag >> 0 & 0b1;
}

void Cpu::SetCarry()
{
  regs_.flag |= 0b00000001;
}

void Cpu::UnsetCarry()
{
  regs_.flag &= 0b11111110;
}

bool Cpu::Zero()
{
  return regs_.flag >> 1 & 0b1;
}

void Cpu::SetZero()
{
  regs_.flag |= 0b00000010;
}

void Cpu::UnsetZero()
{
  regs_.flag &= 0b11111101;
}

bool Cpu::IRQ()
{
  return regs_.flag >> 2 & 0b1;
}

void Cpu::SetIRQ()
{
  regs_.flag |= 0b00000100;
}

void Cpu::UnsetIRQ()
{
  regs_.flag &= 0b11111011;
}

bool Cpu::Decimal()
{
  return regs_.flag >> 3 & 0b1;
}

void Cpu::SetDecimal()
{
  regs_.flag |= 0b00001000;
}

void Cpu::UnsetDecimal()
{
  regs_.flag &= 0b11110111;
}

bool Cpu::Break()
{
  return regs_.flag >> 4 & 0b1;
}

void Cpu::SetBreak()
{
  regs_.flag |= 0b00010000;
}

void Cpu::UnsetBreak()
{
  regs_.flag &= 0b11101111;
}

bool Cpu::Overflow()
{
  return regs_.flag >> 6 & 0b1;
}

void Cpu::SetOverflow()
{
  regs_.flag |= 0b01000000;
}

void Cpu::UnsetOverflow()
{
  regs_.flag &= 0b10111111;
}

bool Cpu::Negative()
{
  return regs_.flag >> 7 & 0b1;
}

void Cpu::SetNegative()
{
  regs_.flag |= 0b10000000;
}

void Cpu::UnsetNegative()
{
  regs_.flag &= 0b01111111;
}

