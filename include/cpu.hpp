#pragma once

#include <cstdint>
#include <cstddef>

#include "bus.hpp"
#include "register.hpp"

using std::size_t;
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

class Cpu {
private:
  Register regs_{};
  Bus& bus_;
  std::array<OpInfo, 256> optable_{};
  void MakeOpTable();
public:
  Cpu(Bus& bus);
  void Start();
};
