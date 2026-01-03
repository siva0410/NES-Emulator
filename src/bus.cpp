#include "bus.hpp"

Bus::Bus(Ram* ram, Rom* rom)
  : ram_(ram), rom_(rom)
{
}

/*
$0000–$07FF 	$0800 	2 KB internal RAM
$0800–$0FFF 	$0800 	Mirrors of $0000–$07FF
$1000–$17FF 	$0800
$1800–$1FFF 	$0800
$2000–$2007 	$0008 	NES PPU registers
$2008–$3FFF 	$1FF8 	Mirrors of $2000–$2007 (repeats every 8 bytes)
$4000–$4017 	$0018 	NES APU and I/O registers
$4018–$401F 	$0008 	APU and I/O functionality that is normally disabled. See CPU Test Mode.
$4020–$FFFF
• $6000–$7FFF
• $8000–$FFFF 	$BFE0
$2000
$8000 	Unmapped. Available for cartridge use.
Usually cartridge RAM, when present.
Usually cartridge ROM and mapper registers.
*/

uint8_t Bus::Read(uint16_t addr) const
{
  addr = addr&0xFFFF;
  if(addr >= 0x0000 && addr < 0x0800){
    return ram_->Read(addr);
  }
  else if(addr >= 0x8000){
    return rom_->Read(addr-0x8000);
  }
  else {
    return ram_->Read(addr);
  }
}

void Bus::Write(uint16_t addr, uint8_t data)
{
  // ram[addr&0xFFFF] = data;
}

void Bus::SetROM(std::string romfile)
{
  rom_->Set(romfile);
}
