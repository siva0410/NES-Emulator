#include "ppubus.hpp"

PpuBus::PpuBus(Ram& vram, Ram& palletram)
  : vram_(vram), palletram_(palletram)
{
}

void PpuBus::SetRom(Rom* rom)
{
  rom_ = rom;
}

uint16_t PpuBus::GetNametableAddr(uint16_t addr) const
{
  uint16_t result{};
  
  addr &= 0x0FFF;
  if (rom_->IsVerticalMirror()) {
    /* VRAM NT0 */
    if (addr >= 0x000 && addr <= 0x3FF) {
      result = addr;
    }
    /* VRAM NT1 */
    else if (addr >= 0x400 && addr <= 0x7FF) {
      result = addr;
    }
    /* VRAM NT2 */
    else if (addr >= 0x800 && addr <= 0xBFF) {
      result = addr - 0x800;
    }
    /* VRAM NT3 */
    else if (addr >= 0xC00 && addr <= 0xFFF) {
      result = addr - 0x800;
    }
  }
  else {
    /* VRAM NT0 */
    if (addr >= 0x000 && addr <= 0x3FF) {
      result =  addr;
    }
    /* VRAM NT1 */
    else if (addr >= 0x400 && addr <= 0x7FF) {
      result =  addr - 0x400;
    }
    /* VRAM NT2 */
    else if (addr >= 0x800 && addr <= 0xBFF) {
      result = addr - 0x400;
    }
    /* VRAM NT3 */
    else if (addr >= 0xC00 && addr <= 0xFFF) {
      result =  addr - 0x800;
    }
  }
  return result;
}

uint8_t PpuBus::Read(uint16_t addr) const
{
  addr &= 0x3FFF;

  /* CHR_ROM */
  if(addr >= 0x0000 && addr <= 0x1FFF) {
    if(rom_->IsChrRam()) {
      return rom_->ReadChrRam(addr);
    }
    return rom_->ReadChrRom(addr);
  }
  /* Nametables */
  else if ((addr >= 0x2000 && addr <= 0x2FFF) || (addr >=0x3000 && addr <= 0x3EFF)) {
    return vram_.Read(GetNametableAddr(addr));
  }
  /* Pallet RAM and Mirros */
  else if(addr >= 0x3F00 && addr <= 0x3FFF) {
    addr = addr&0x001F;
    if(addr == 0x10) addr = 0x00;
    if(addr == 0x14) addr = 0x04;
    if(addr == 0x18) addr = 0x08;
    if(addr == 0x1C) addr = 0x0C;
    return palletram_.Read(addr);
  }
  else {
    throw std::runtime_error("Out of VRAM. (read)");
  }
}

void PpuBus::Write(uint16_t addr, uint8_t data)
{
  addr &= 0x3FFF;

  /* CHR_RAM */
  if(addr >= 0x0000 && addr <= 0x1FFF) {
    if(rom_->IsChrRam()) {
      rom_->WriteChrRam(addr, data);
    }
  }
  /* Nametables */
  else if ((addr >= 0x2000 && addr <= 0x2FFF) || (addr >=0x3000 && addr <= 0x3EFF)) {
    vram_.Write(GetNametableAddr(addr), data);
  }
  /* Pallet RAM and Mirros */
  else if (addr >= 0x3F00 && addr <= 0x3FFF) {
    addr = addr&0x001F;
    if(addr == 0x10) addr = 0x00;
    if(addr == 0x14) addr = 0x04;
    if(addr == 0x18) addr = 0x08;
    if(addr == 0x1C) addr = 0x0C;
    palletram_.Write(addr, data);
  }
  else {
    throw std::runtime_error("Out of VRAM. (write)");
  }
}
