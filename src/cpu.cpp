#include <iostream>
#include "cpu.hpp"

using enum AddrMode;
using enum Opcode;

Cpu::Cpu(CpuBus& cpubus)
  : cpubus_(cpubus)
{
  MakeOpTable();
}

void Cpu::Reset()
{
  regs_.pc = cpubus_.Read(0xFFFC) | (cpubus_.Read(0XFFFD)<<8);
  SetIRQ();
}

void Cpu::Tick()
{
  if(cycles_ > 0) {
    cycles_--;
    return;
  }
    
  uint8_t idx{cpubus_.Read(regs_.pc++)};
  cycles_ = optable_.at(idx).cycles;
  
  std::cout << std::hex << std::setw(4) << std::setfill('0')
	    << static_cast<unsigned int>(regs_.pc-1) << ": "
	    << optable_[idx].mnemonic << ": ";

  uint16_t operand{};
  uint16_t addr{};
  switch(optable_.at(idx).mode) {
  case Imp:
    break;
    
  case Acc:
    break;
    
  case Imm:
    operand = cpubus_.Read(regs_.pc++);
    break;
    
  case Zp:
    operand = cpubus_.Read(regs_.pc++);
    break;
    
  case ZpX:
    operand = cpubus_.Read(regs_.pc++) + regs_.x;
    break;
    
  case ZpY:
    operand = cpubus_.Read(regs_.pc++) + regs_.y;
    break;
    
  case Abs:
    operand = cpubus_.Read(regs_.pc++) | (cpubus_.Read(regs_.pc++)<<8);
    break;
    
  case AbsX:
    operand = (cpubus_.Read(regs_.pc++) | (cpubus_.Read(regs_.pc++)<<8)) + regs_.x;
    break;
    
  case AbsY:
    operand = (cpubus_.Read(regs_.pc++) | (cpubus_.Read(regs_.pc++)<<8)) + regs_.y;
    break;
    
  case Ind:
    addr = cpubus_.Read(regs_.pc++) | (cpubus_.Read(regs_.pc++)<<8);
    if((addr&0xFF) == 0xFF){
      operand = cpubus_.Read(addr) | (cpubus_.Read(addr&0xFF00)<<8);
    } else {
      operand = cpubus_.Read(addr) | (cpubus_.Read(addr+1)<<8);
    }
    break;
    
  case IndX:
    addr = (cpubus_.Read(regs_.pc++) | (cpubus_.Read(regs_.pc++)<<8)) + regs_.x;
    operand = cpubus_.Read(addr) | (cpubus_.Read(addr+1)<<8);
    break;
    
  case IndY:
    addr = cpubus_.Read(regs_.pc++) | (cpubus_.Read(regs_.pc++)<<8);
    operand = (cpubus_.Read(addr) | (cpubus_.Read(addr+1)<<8)) + regs_.y;
    break;
    
  case Rel:
    operand = cpubus_.Read(regs_.pc++);
    break;
    
  default:
    break;
  }

  std::cout << static_cast<unsigned int>(operand) << std::endl;
  
  uint8_t res{};
  switch(optable_.at(idx).opcode) {
  case LDA:
    if(optable_.at(idx).mode == Imm) {
      regs_.a = operand;
    }
    else {
      regs_.a = cpubus_.Read(operand);
    }
    UpdateZeroFlag(regs_.a);
    UpdateNegativeFlag(regs_.a);
    break;
    
  case LDX:
    if(optable_.at(idx).mode == Imm) {
      regs_.x = operand;
    }
    else {
      regs_.x = cpubus_.Read(operand);
    }
    UpdateZeroFlag(regs_.x);
    UpdateNegativeFlag(regs_.x);
    break;
    
  case LDY:
    if(optable_.at(idx).mode == Imm) {
      regs_.y = operand;
    }
    else {
      regs_.y = cpubus_.Read(operand);
    }
    UpdateZeroFlag(regs_.y);
    UpdateNegativeFlag(regs_.y);
    break;
    
  case STA:
    cpubus_.Write(operand, regs_.a);
    break;
    
  case STX:
    cpubus_.Write(operand, regs_.x);
    break;
    
  case STY:
    cpubus_.Write(operand, regs_.y);
    break;
    
  case TAX:
    regs_.x = regs_.a;
    UpdateZeroFlag(regs_.x);
    UpdateNegativeFlag(regs_.x);
    break;
    
  case TAY:
    regs_.y = regs_.a;
    UpdateZeroFlag(regs_.y);
    UpdateNegativeFlag(regs_.y);

  case TSX:
    regs_.x = regs_.s;
    UpdateZeroFlag(regs_.x);
    UpdateNegativeFlag(regs_.x);
    break;
    
  case TXA:
    regs_.a = regs_.x;
    UpdateZeroFlag(regs_.a);
    UpdateNegativeFlag(regs_.a);
    break;
    
  case TXS:
    regs_.s = regs_.x;
    UpdateZeroFlag(regs_.s);
    UpdateNegativeFlag(regs_.s);
    break;

  case TYA:
    regs_.a = regs_.y;
    UpdateZeroFlag(regs_.a);
    UpdateNegativeFlag(regs_.a);
    break;
    
  case ADC:
  case AND:
  case ASL:
  case BIT:
  case CMP:
  case CPX:
  case CPY:
    break;
    
  case DEC:
    res = cpubus_.Read(operand)-1;
    cpubus_.Write(operand, res);
    UpdateZeroFlag(res);
    UpdateNegativeFlag(res);
    break;
    
  case DEX:
    regs_.x--;
    UpdateZeroFlag(regs_.x);
    UpdateNegativeFlag(regs_.x);
    break;
    
  case DEY:
    regs_.y--;
    UpdateZeroFlag(regs_.y);
    UpdateNegativeFlag(regs_.y);
    break;
    
  case EOR:
    break;
    
  case INC:
    res = cpubus_.Read(operand)+1;
    cpubus_.Write(operand, res);
    UpdateZeroFlag(res);
    UpdateNegativeFlag(res);
    break;
    
  case INX:
    regs_.x++;
    UpdateZeroFlag(regs_.x);
    UpdateNegativeFlag(regs_.x);
    break;
    
  case INY:
    regs_.y++;
    UpdateZeroFlag(regs_.y);
    UpdateNegativeFlag(regs_.y);
    break;
    
  case LSR:
  case ORA:
  case ROL:
  case ROR:
  case SBC:
  case PHA:
  case PHP:
  case PLA:
  case PLP:
    break;
    
  case JMP:
    regs_.pc = operand;
    break;
    
  case JSR:
  case RTS:
  case RTI:
    break;
    
  case BCC:
    if(!Carry()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BCS:
    if(Carry()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BEQ:
    if(Zero()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BMI:
    if(Negative()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BNE:
    if(!Zero()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BPL:
    if(!Negative()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BVC:
    if(!Overflow()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case BVS:
    if(Overflow()) regs_.pc += static_cast<int8_t>(operand);
    break;
    
  case CLC:
    UnsetCarry();
    break;
    
  case CLD:
    UnsetDecimal();
    break;
    
  case CLI:
    UnsetIRQ();
    break;
    
  case CLV:
    UnsetOverflow();
    break;
    
  case SEC:
    SetCarry();
    break;
    
  case SED:
    SetDecimal();
    break;
    
  case SEI:
    SetIRQ();
    break;
    
  case BRK:
    SetBreak();
    break;
    
  case NOP:
    break;
    
  default:
    break;
  }
}

void Cpu::MakeOpTable()
{
  // { mnemonic, opcode, mode, size, cycles}
  optable_.fill({ "???", Opcode::INVALID, AddrMode::Imp, 1, 0 });

  optable_[0xA9] = { "LDA", Opcode::LDA, AddrMode::Imm, 2, 2};
  optable_[0xA5] = { "LDA", Opcode::LDA, AddrMode::Zp, 2, 3};
  optable_[0xB5] = { "LDA", Opcode::LDA, AddrMode::ZpX, 2, 4};
  optable_[0xAD] = { "LDA", Opcode::LDA, AddrMode::Abs, 3, 4};
  optable_[0xBD] = { "LDA", Opcode::LDA, AddrMode::AbsX, 3, 4};
  optable_[0xB9] = { "LDA", Opcode::LDA, AddrMode::AbsY, 3, 4};
  optable_[0xA1] = { "LDA", Opcode::LDA, AddrMode::IndX, 2, 6};
  optable_[0xB1] = { "LDA", Opcode::LDA, AddrMode::IndY, 2, 5};

  optable_[0xA2] = { "LDX", Opcode::LDX, AddrMode::Imm, 2, 2};
  optable_[0xA6] = { "LDX", Opcode::LDX, AddrMode::Zp, 2, 3};
  optable_[0xB6] = { "LDX", Opcode::LDX, AddrMode::ZpY, 2, 4};
  optable_[0xAE] = { "LDX", Opcode::LDX, AddrMode::Abs, 3, 4};
  optable_[0xBE] = { "LDX", Opcode::LDX, AddrMode::AbsY, 3, 4};

  optable_[0xA0] = { "LDY", Opcode::LDY, AddrMode::Imm, 2, 2};
  optable_[0xA4] = { "LDY", Opcode::LDY, AddrMode::Zp, 2, 3};
  optable_[0xB4] = { "LDY", Opcode::LDY, AddrMode::ZpX, 2, 4};
  optable_[0xAC] = { "LDY", Opcode::LDY, AddrMode::Abs, 3, 4};
  optable_[0xBC] = { "LDY", Opcode::LDY, AddrMode::AbsX, 3, 4};

  optable_[0x85] = { "STA", Opcode::STA, AddrMode::Zp, 2, 3};
  optable_[0x95] = { "STA", Opcode::STA, AddrMode::ZpX, 2, 4};
  optable_[0x8D] = { "STA", Opcode::STA, AddrMode::Abs, 3, 4};
  optable_[0x9D] = { "STA", Opcode::STA, AddrMode::AbsX, 3, 5};
  optable_[0x99] = { "STA", Opcode::STA, AddrMode::AbsY, 3, 5};
  optable_[0x81] = { "STA", Opcode::STA, AddrMode::IndX, 2, 6};
  optable_[0x91] = { "STA", Opcode::STA, AddrMode::IndY, 2, 6};

  optable_[0x86] = { "STX", Opcode::STX, AddrMode::Zp, 2, 3};
  optable_[0x96] = { "STX", Opcode::STX, AddrMode::ZpY, 2, 4};
  optable_[0x8E] = { "STX", Opcode::STX, AddrMode::Abs, 3, 4};

  optable_[0x84] = { "STY", Opcode::STY, AddrMode::Zp, 2, 3};
  optable_[0x94] = { "STY", Opcode::STY, AddrMode::ZpX, 2, 4};
  optable_[0x8C] = { "STY", Opcode::STY, AddrMode::Abs, 3, 4};

  optable_[0xAA] = { "TAX", Opcode::TAX, AddrMode::Imp, 1, 2};

  optable_[0xA8] = { "TAY", Opcode::TAY, AddrMode::Imp, 1, 2};

  optable_[0xBA] = { "TSX", Opcode::TSX, AddrMode::Imp, 1, 2};

  optable_[0x8A] = { "TXA", Opcode::TXA, AddrMode::Imp, 1, 2};

  optable_[0x9A] = { "TXS", Opcode::TXS, AddrMode::Imp, 1, 2};

  optable_[0x98] = { "TYA", Opcode::TYA, AddrMode::Imp, 1, 2};

  optable_[0x69] = { "ADC", Opcode::ADC, AddrMode::Imm, 2, 2};
  optable_[0x65] = { "ADC", Opcode::ADC, AddrMode::Zp, 2, 3};
  optable_[0x75] = { "ADC", Opcode::ADC, AddrMode::ZpX, 2, 4};
  optable_[0x6D] = { "ADC", Opcode::ADC, AddrMode::Abs, 3, 4};
  optable_[0x7D] = { "ADC", Opcode::ADC, AddrMode::AbsX, 3, 4};
  optable_[0x79] = { "ADC", Opcode::ADC, AddrMode::AbsY, 3, 4};
  optable_[0x61] = { "ADC", Opcode::ADC, AddrMode::IndX, 2, 6};
  optable_[0x71] = { "ADC", Opcode::ADC, AddrMode::IndY, 2, 5};
  
  optable_[0x29] = { "AND", Opcode::AND, AddrMode::Imm, 2, 2};
  optable_[0x25] = { "AND", Opcode::AND, AddrMode::Zp, 2, 3};
  optable_[0x35] = { "AND", Opcode::AND, AddrMode::ZpX, 2, 4};
  optable_[0x2D] = { "AND", Opcode::AND, AddrMode::Abs, 3, 4};
  optable_[0x3D] = { "AND", Opcode::AND, AddrMode::AbsX, 3, 4};
  optable_[0x39] = { "AND", Opcode::AND, AddrMode::AbsY, 3, 4};
  optable_[0x21] = { "AND", Opcode::AND, AddrMode::IndX, 2, 6};
  optable_[0x31] = { "AND", Opcode::AND, AddrMode::IndY, 2, 5};

  optable_[0x0A] = { "ASL", Opcode::ASL, AddrMode::Acc, 1, 2};
  optable_[0x06] = { "ASL", Opcode::ASL, AddrMode::Zp, 2, 5};
  optable_[0x16] = { "ASL", Opcode::ASL, AddrMode::ZpX, 2, 6};
  optable_[0x0E] = { "ASL", Opcode::ASL, AddrMode::Abs, 3, 6};
  optable_[0x1E] = { "ASL", Opcode::ASL, AddrMode::AbsX, 3, 7};

  optable_[0x24] = { "BIT", Opcode::BIT, AddrMode::Zp, 2, 3};
  optable_[0x2C] = { "BIT", Opcode::BIT, AddrMode::Zp, 3, 4};

  optable_[0xC9] = { "CMP", Opcode::CMP, AddrMode::Imm, 2, 2};
  optable_[0xC5] = { "CMP", Opcode::CMP, AddrMode::Zp, 2, 3};
  optable_[0xD5] = { "CMP", Opcode::CMP, AddrMode::ZpX, 2, 4};
  optable_[0xCD] = { "CMP", Opcode::CMP, AddrMode::Abs, 3, 4};
  optable_[0xDD] = { "CMP", Opcode::CMP, AddrMode::AbsX, 3, 4};
  optable_[0xD9] = { "CMP", Opcode::CMP, AddrMode::AbsY, 3, 4};
  optable_[0xC1] = { "CMP", Opcode::CMP, AddrMode::IndX, 2, 6};
  optable_[0xD1] = { "CMP", Opcode::CMP, AddrMode::IndY, 2, 5};

  optable_[0xE0] = { "CPX", Opcode::CPX, AddrMode::Imm, 2, 2};
  optable_[0xE4] = { "CPX", Opcode::CPX, AddrMode::Zp, 2, 3};
  optable_[0xEC] = { "CPX", Opcode::CPX, AddrMode::Abs, 3, 4};

  optable_[0xC0] = { "CPY", Opcode::CPY, AddrMode::Imm, 2, 2};
  optable_[0xC4] = { "CPY", Opcode::CPY, AddrMode::Zp, 2, 3};
  optable_[0xCC] = { "CPY", Opcode::CPY, AddrMode::Abs, 3, 4};

  optable_[0xC6] = { "DEC", Opcode::DEC, AddrMode::Zp, 2, 5};
  optable_[0xD6] = { "DEC", Opcode::DEC, AddrMode::ZpX, 2, 6};
  optable_[0xCE] = { "DEC", Opcode::DEC, AddrMode::Abs, 3, 6};
  optable_[0xDE] = { "DEC", Opcode::DEC, AddrMode::AbsX, 3, 7};

  optable_[0xCA] = { "DEX", Opcode::DEX, AddrMode::Imp, 1, 2};

  optable_[0x88] = { "DEY", Opcode::DEY, AddrMode::Imp, 1, 2};

  optable_[0x49] = { "EOR", Opcode::EOR, AddrMode::Imm, 2, 2};
  optable_[0x45] = { "EOR", Opcode::EOR, AddrMode::Zp, 2, 3};
  optable_[0x45] = { "EOR", Opcode::EOR, AddrMode::ZpX, 2, 4};
  optable_[0x4D] = { "EOR", Opcode::EOR, AddrMode::Abs, 3, 4};
  optable_[0x5D] = { "EOR", Opcode::EOR, AddrMode::AbsX, 3, 4};
  optable_[0x59] = { "EOR", Opcode::EOR, AddrMode::AbsY, 3, 4};
  optable_[0x41] = { "EOR", Opcode::EOR, AddrMode::IndX, 2, 6};
  optable_[0x51] = { "EOR", Opcode::EOR, AddrMode::IndY, 2, 5};

  optable_[0xE6] = { "INC", Opcode::INC, AddrMode::Zp, 2, 5};
  optable_[0xF6] = { "INC", Opcode::INC, AddrMode::ZpX, 2, 6};
  optable_[0xEE] = { "INC", Opcode::INC, AddrMode::Abs, 3, 6};
  optable_[0xFE] = { "INC", Opcode::INC, AddrMode::AbsX, 3, 7};

  optable_[0xE8] = { "INX", Opcode::INX, AddrMode::Imp, 1, 2};

  optable_[0xC8] = { "INY", Opcode::INY, AddrMode::Imp, 1, 2};

  optable_[0x4A] = { "LSR", Opcode::LSR, AddrMode::Acc, 1, 2};
  optable_[0x46] = { "LSR", Opcode::LSR, AddrMode::Zp, 2, 5};
  optable_[0x56] = { "LSR", Opcode::LSR, AddrMode::ZpX, 2, 6};
  optable_[0x4E] = { "LSR", Opcode::LSR, AddrMode::Abs, 3, 6};
  optable_[0x5E] = { "LSR", Opcode::LSR, AddrMode::AbsX, 3, 7};

  optable_[0x09] = { "ORA", Opcode::ORA, AddrMode::Imm, 2, 2};
  optable_[0x05] = { "ORA", Opcode::ORA, AddrMode::Zp, 2, 3};
  optable_[0x15] = { "ORA", Opcode::ORA, AddrMode::ZpX, 2, 4};
  optable_[0x0D] = { "ORA", Opcode::ORA, AddrMode::Abs, 3, 4};
  optable_[0x1D] = { "ORA", Opcode::ORA, AddrMode::AbsX, 3, 4};
  optable_[0x19] = { "ORA", Opcode::ORA, AddrMode::AbsY, 3, 4};
  optable_[0x01] = { "ORA", Opcode::ORA, AddrMode::IndX, 2, 6};
  optable_[0x11] = { "ORA", Opcode::ORA, AddrMode::IndY, 2, 5};

  optable_[0x6A] = { "ROR", Opcode::ROR, AddrMode::Acc, 1, 2};
  optable_[0x66] = { "ROR", Opcode::ROR, AddrMode::Zp, 2, 5};
  optable_[0x76] = { "ROR", Opcode::ROR, AddrMode::ZpX, 2, 6};
  optable_[0x6E] = { "ROR", Opcode::ROR, AddrMode::Abs, 3, 6};
  optable_[0x7E] = { "ROR", Opcode::ROR, AddrMode::AbsX, 3, 7};

  optable_[0xE9] = { "SBC", Opcode::SBC, AddrMode::Imm, 2, 2};
  optable_[0xE5] = { "SBC", Opcode::SBC, AddrMode::Zp, 2, 3};
  optable_[0xF5] = { "SBC", Opcode::SBC, AddrMode::ZpX, 2, 4};
  optable_[0xED] = { "SBC", Opcode::SBC, AddrMode::Abs, 3, 4};
  optable_[0xFD] = { "SBC", Opcode::SBC, AddrMode::AbsX, 3, 4};
  optable_[0xF9] = { "SBC", Opcode::SBC, AddrMode::AbsY, 3, 4};
  optable_[0xE1] = { "SBC", Opcode::SBC, AddrMode::IndX, 2, 6};
  optable_[0xF1] = { "SBC", Opcode::SBC, AddrMode::IndY, 2, 5};

  optable_[0x48] = { "PHA", Opcode::PHA, AddrMode::Imp, 1, 3};
  
  optable_[0x08] = { "PHP", Opcode::PHP, AddrMode::Imp, 1, 3};
  
  optable_[0x68] = { "PLA", Opcode::PLA, AddrMode::Imp, 1, 4};

  optable_[0x28] = { "PHP", Opcode::PHP, AddrMode::Imp, 1, 4};

  optable_[0x4C] = { "JMP", Opcode::JMP, AddrMode::Abs, 3, 3};
  optable_[0x6C] = { "JMP", Opcode::JMP, AddrMode::Ind, 3, 5};

  optable_[0x20] = { "JSR", Opcode::JSR, AddrMode::Abs, 3, 6};

  optable_[0x60] = { "RTS", Opcode::RTS, AddrMode::Imp, 1, 6};

  optable_[0x40] = { "RTI", Opcode::RTI, AddrMode::Imp, 1, 6};

  optable_[0x90] = { "BCC", Opcode::BCC, AddrMode::Rel, 2, 2};

  optable_[0xB0] = { "BCS", Opcode::BCS, AddrMode::Rel, 2, 2};

  optable_[0xF0] = { "BEQ", Opcode::BEQ, AddrMode::Rel, 2, 2};

  optable_[0x30] = { "BMI", Opcode::BMI, AddrMode::Rel, 2, 2};

  optable_[0xD0] = { "BNE", Opcode::BNE, AddrMode::Rel, 2, 2};

  optable_[0x10] = { "BPL", Opcode::BPL, AddrMode::Rel, 2, 2};

  optable_[0x50] = { "BVC", Opcode::BVC, AddrMode::Rel, 2, 2};

  optable_[0x70] = { "BVS", Opcode::BVS, AddrMode::Rel, 2, 2};

  optable_[0x18] = { "CLC", Opcode::CLC, AddrMode::Imp, 1, 2};

  optable_[0xD8] = { "CLD", Opcode::CLD, AddrMode::Imp, 1, 2};

  optable_[0x58] = { "CLI", Opcode::CLI, AddrMode::Imp, 1, 2};

  optable_[0xB8] = { "CLV", Opcode::CLV, AddrMode::Imp, 1, 2};

  optable_[0x38] = { "SEC", Opcode::SEC, AddrMode::Imp, 1, 2};

  optable_[0xF8] = { "SED", Opcode::SED, AddrMode::Imp, 1, 2};

  optable_[0x78] = { "SEI", Opcode::SEI, AddrMode::Imp, 1, 2};

  optable_[0x00] = { "BRK", Opcode::BRK, AddrMode::Imp, 1, 7};

  optable_[0xEA] = { "NOP", Opcode::NOP, AddrMode::Imp, 1, 2};
}
