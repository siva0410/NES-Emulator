#pragma once

#include <cstdint>

#include "cpubus.hpp"

using std::int8_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

enum class Opcode {
  LDA, LDX, LDY,
  STA, STX, STY,
  TAX, TAY,
  TSX, TXA, TXS, TYA,
  ADC, AND, ASL,
  BIT,
  CMP, CPX, CPY,
  DEC, DEX, DEY,
  INC, INX, INY,
  EOR,
  LSR, ORA, ROL, ROR,
  SBC,
  PHA, PHP, PLA, PLP,
  JMP, JSR,
  RTS, RTI,
  BCC, BCS, BEQ, BMI,
  BNE, BPL, BVC, BVS,
  CLC, CLD, CLI, CLV,
  SEC, SED, SEI,
  BRK,
  NOP,
  INVALID,
};


enum class AddrMode {
  Imp,
  Acc,
  Imm,
  Zp,
  ZpX,
  ZpY,
  Abs,
  AbsX,
  AbsY,
  Ind,
  IndX,
  IndY,
  Rel,
};

struct OpInfo {
  const char* mnemonic;
  Opcode opcode;
  AddrMode mode;
  uint8_t size;
  uint8_t cycles;
};

struct Register {
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint8_t s = 0xFD;
  uint16_t pc;
  uint8_t flag = 0b00100000;
};

class Cpu {
private:
  uint32_t cycles_{};
  bool nmiRequest_{};
  CpuBus& cpubus_;
  Register regs_{};
  std::array<OpInfo, 256> optable_{};
  void Push(uint8_t value);
  void Push16(uint16_t value);
  uint8_t Pull();
  uint16_t Pull16();
  void MakeOpTable();
  bool Carry();
  void SetCarry();
  void UnsetCarry();
  bool Zero();
  void SetZero();
  void UnsetZero();
  bool IRQ();
  void SetIRQ();
  void UnsetIRQ();
  bool Decimal();
  void SetDecimal();
  void UnsetDecimal();
  bool Break();
  void SetBreak();
  void UnsetBreak();
  bool Overflow();
  void SetOverflow();
  void UnsetOverflow();
  bool Negative();
  void SetNegative();
  void UnsetNegative();
  void UpdateZeroFlag(uint8_t data);
  void UpdateAdcOverflowFlag(uint8_t arg1, uint8_t arg2, uint16_t sum);
  void UpdateSbcOverflowFlag(uint8_t arg1, uint8_t arg2, uint16_t sum);
  void UpdateNegativeFlag(uint8_t data);
public:
  Cpu(CpuBus& cpubus);
  void Nmi();
  void RequestNmi();
  void Reset();
  void Irq();
  void Clock();
};
