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
  /* CHR_ROM */
  if(addr >= 0x0000 && addr <= 0x1FFF) {
    return rom_->ReadChrRom(addr);
  }
  /* VRAM NT0 */
  else if(addr >= 0x2000 && addr <= 0x23FF) {
    return vram_.Read(addr&0x07FF);
  }
  /* VRAM NT1 */
  else if(addr >= 0x2400 && addr <= 0x27FF) {
    return vram_.Read(addr&0x07FF);
  }
  /* VRAM NT2 Vertical Mirror */
  else if(addr >= 0x2800 && addr <= 0x2BFF) {
    return vram_.Read(addr&0x07FF);
  }
  /* VRAM NT3 Vertical Mirror */
  else if(addr >= 0x2C00 && addr <= 0x2FFF) {
    return vram_.Read(addr&0x07FF);
  }
  /* Pallet RAM and Mirros */
  else if(addr >= 0x3F00 && addr <= 0x3FFF) {
    return palletram_.Read(addr&0x001F);
  }
  else {
    throw std::runtime_error("Out of VRAM. (read)");
  }
}

void PpuBus::Write(uint16_t addr, uint8_t data)
{
  /* VRAM NT0 */
  if(addr >= 0x2000 && addr <= 0x23FF) {
    vram_.Write(addr&0x07FF, data);
  }
  /* VRAM NT1 */
  else if(addr >= 0x2400 && addr <= 0x27FF) {
    vram_.Write(addr&0x07FF, data);
  }
  /* VRAM NT2 Vertical Mirror */
  else if(addr >= 0x2800 && addr <= 0x2BFF) {
    vram_.Write(addr&0x07FF, data);
  }
  /* VRAM NT3 Vertical Mirror */
  else if(addr >= 0x2C00 && addr <= 0x2FFF) {
    vram_.Write(addr&0x07FF, data);
  }
  /* Pallet RAM and Mirros */
  else if(addr >= 0x3F00 && addr <= 0x3FFF) {
    palletram_.Write(addr&0x001F, data);
  }
  else {
    throw std::runtime_error("Out of VRAM. (write)");
  }
}
