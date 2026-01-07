#include <iostream>
#include "cpu.hpp"

Cpu::Cpu(Bus& bus)
  : bus_(bus)
{
  MakeOpTable();
}

void Cpu::Start()
{
  regs_.SetPC(0);
  uint8_t idx = bus_.Read(regs_.PC());
  switch(optable_[idx].opcode) {
  case Opcode::LDA:
    std::cout << "LDA" << std::endl;
    break;
  default:
    break;
  }
}

void Cpu::MakeOpTable()
{
  // { mnemonic, opcode, mode, size, cycles}
  optable_.fill({ "???", Opcode::INVALID, AddrMode::Imp, 1, 0 });

  optable_[0xA9] = { "LDA", Opcode::INVALID, AddrMode::Imp, 2, 2};
  
}
