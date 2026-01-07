#include "register.hpp"

void Register::SetPC(uint16_t addr)
{
  pc_ = addr;
}

void Register::IncPC()
{
  pc_++;
}

uint16_t Register::PC()
{
  return pc_;
}

uint8_t Register::A()
{
  return a_;
}

void Register::SetA(uint8_t data)
{
  a_ = data;
}

uint8_t Register::X()
{
  return x_;
}

void Register::SetX(uint8_t data)
{
  x_ = data;
}

uint8_t Register::Y()
{
  return y_;
}

void Register::SetY(uint8_t data)
{
  y_ = data;
}

uint8_t Register::S()
{
  return s_;
}

void Register::SetS(uint8_t data)
{
  s_ = data;
}

uint8_t Register::P()
{
  return p_;
}

void Register::SetP(uint8_t data)
{
  p_ = data;
}

void Register::SetCarry()
{
  flag_ |= 0b00000001;
}

void Register::UnsetCarry()
{
  flag_ &= 0b11111110;
}

void Register::SetZero()
{
  flag_ |= 0b00000010;
}

void Register::UnsetZero()
{
  flag_ &= 0b11111101;
}

void Register::SetIRQ()
{
  flag_ |= 0b00000100;
}

void Register::UnsetIRQ()
{
  flag_ &= 0b11111011;
}

void Register::SetDecimal()
{
  flag_ |= 0b00001000;
}

void Register::UnsetDecimal()
{
  flag_ &= 0b11110111;
}

void Register::SetBreak()
{
  flag_ |= 0b00010000;
}

void Register::UnsetBreak()
{
  flag_ &= 0b11101111;
}

void Register::SetOverflow()
{
  flag_ |= 0b01000000;
}

void Register::UnsetOverflow()
{
  flag_ &= 0b10111111;
}

void Register::SetNegative()
{
  flag_ |= 0b10000000;
}

void Register::UnsetNegative()
{
  flag_ &= 0b01111111;
}

