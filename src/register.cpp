#include "register.hpp"

void Register::SetPC(uint16_t addr)
{
  pc = addr;
}

void Register::IncPC()
{
  pc++;
}

uint16_t Register::PC()
{
  return pc;
}

uint8_t Register::A()
{
  return a;
}

void Register::SetA(uint8_t data)
{
  a = data;
}

uint8_t Register::X()
{
  return x;
}

void Register::SetX(uint8_t data)
{
  x = data;
}

uint8_t Register::Y()
{
  return y;
}

void Register::SetY(uint8_t data)
{
  y = data;
}

uint8_t Register::S()
{
  return s;
}

void Register::SetS(uint8_t data)
{
  s = data;
}

uint8_t Register::P()
{
  return p;
}

void Register::SetP(uint8_t data)
{
  p = data;
}

void Register::SetCarry()
{
  flag |= 0b00000001;
}

void Register::UnsetCarry()
{
  flag &= 0b11111110;
}

void Register::SetZero()
{
  flag |= 0b00000010;
}

void Register::UnsetZero()
{
  flag &= 0b11111101;
}

void Register::SetIRQ()
{
  flag |= 0b00000100;
}

void Register::UnsetIRQ()
{
  flag &= 0b11111011;
}

void Register::SetDecimal()
{
  flag |= 0b00001000;
}

void Register::UnsetDecimal()
{
  flag &= 0b11110111;
}

void Register::SetBreak()
{
  flag |= 0b00010000;
}

void Register::UnsetBreak()
{
  flag &= 0b11101111;
}

void Register::SetOverflow()
{
  flag |= 0b01000000;
}

void Register::UnsetOverflow()
{
  flag &= 0b10111111;
}

void Register::SetNegative()
{
  flag |= 0b10000000;
}

void Register::UnsetNegative()
{
  flag &= 0b01111111;
}

