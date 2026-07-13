#include "ppubus.hpp"

PpuBus::PpuBus(Ram& vram, Ram& palletram)
  : vram_(vram), palletram_(palletram)
{
}

void PpuBus::SetRom(Rom* rom)
{
  rom_ = rom;
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
  else if ((addr >= 0x2000 && addr <= 0x2FFF) || (addr >=0x3000 && addr <= 0x3EFF)) {
    addr = addr & 0x0FFF;
    if (rom_->IsVerticalMirror()) {
      /* VRAM NT0 */
      if (addr >= 0x000 && addr <= 0x3FF) {
	return vram_.Read(addr);
      }
      /* VRAM NT1 */
      else if (addr >= 0x400 && addr <= 0x7FF) {
	return vram_.Read(addr);
      }
      /* VRAM NT2 */
      else if (addr >= 0x800 && addr <= 0xBFF) {
	return vram_.Read(addr - 0x800);
      }
      /* VRAM NT3 */
      else if (addr >= 0xC00 && addr <= 0xFFF) {
	return vram_.Read(addr - 0x800);
      }
    }
    else {
      /* VRAM NT0 */
      if (addr >= 0x000 && addr <= 0x3FF) {
	return vram_.Read(addr);
      }
      /* VRAM NT1 */
      else if (addr >= 0x400 && addr <= 0x7FF) {
	return vram_.Read(addr - 0x400);
      }
      /* VRAM NT2 */
      else if (addr >= 0x800 && addr <= 0xBFF) {
	return vram_.Read(addr - 0x400);
      }
      /* VRAM NT3 */
      else if (addr >= 0xC00 && addr <= 0xFFF) {
	return vram_.Read(addr - 0x800);
      }
    }
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
  if(addr >= 0x0000 && addr <= 0x1FFF) {
    if(rom_->IsChrRam()) {
      rom_->WriteChrRam(addr, data);
    }
  }
  else if ((addr >= 0x2000 && addr <= 0x2FFF) || (addr >=0x3000 && addr <= 0x3EFF)) {
    addr = addr & 0x0FFF;
    if (rom_->IsVerticalMirror()) {
      /* VRAM NT0 */
      if (addr >= 0x000 && addr <= 0x3FF) {
	vram_.Write(addr, data);
      }
      /* VRAM NT1 */
      else if (addr >= 0x400 && addr <= 0x7FF) {
	vram_.Write(addr, data);
      }
      /* VRAM NT2 */
      else if (addr >= 0x800 && addr <= 0xBFF) {
	vram_.Write(addr - 0x800, data);
      }
      /* VRAM NT3 */
      else if (addr >= 0xC00 && addr <= 0xFFF) {
	vram_.Write(addr - 0x800, data);
      }
    }
    else {
      /* VRAM NT0 */
      if (addr >= 0x000 && addr <= 0x3FF) {
	vram_.Write(addr, data);
      }
      /* VRAM NT1 */
      else if (addr >= 0x400 && addr <= 0x7FF) {
	vram_.Write(addr - 0x400, data);
      }
      /* VRAM NT2 */
      else if (addr >= 0x800 && addr <= 0xBFF) {
	vram_.Write(addr- 0x400, data);
      }
      /* VRAM NT3 */
      else if (addr >= 0xC00 && addr <= 0xFFF) {
	vram_.Write(addr - 0x800, data);
      }
    }
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
