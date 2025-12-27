#include <ios>
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <array>
#include <iomanip>

class Rom {
private:
  std::array<unsigned char, 0x8000> rom{};
public:
  void Set(std::string romfile);
  unsigned char Read(unsigned int addr) const;
};

void Rom::Set(std::string romfile)
{
  std::ifstream in{romfile, std::ios::binary};
  in.read(reinterpret_cast<char*>(rom.data()), static_cast<std::streamsize>(rom.size()));  
}

unsigned char Rom::Read(unsigned int addr) const
{
  return rom.at(addr);
}

class Ram {
private:
  std::array<unsigned char, 0x10000> ram{};
public:
  unsigned char Read(unsigned int addr) const;
  void Write(unsigned int addr, unsigned char data);
};

unsigned char Ram::Read(unsigned int addr) const
{
  return ram.at(addr);
}

void Ram::Write(unsigned int addr, unsigned char data)
{
  ram.at(addr) = data;
}

class Bus {
private:
  Ram* ram_;
  Rom* rom_;
public:
  Bus(Ram* ram, Rom* rom);
  unsigned char Read(unsigned int addr) const;
  void Write(unsigned int addr, unsigned char data);
  void SetROM(std::string romfile);
};

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

unsigned char Bus::Read(unsigned int addr) const
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

void Bus::Write(unsigned int addr, unsigned char data)
{
  // ram[addr&0xFFFF] = data;
}

void Bus::SetROM(std::string romfile)
{
  rom_->Set(romfile);
}

class Cpu {
private:
  Bus* bus_;
public:
  Cpu(Bus* bus);
  void Start();
};

Cpu::Cpu(Bus* bus)
  : bus_(bus)
{
}

void Cpu::Start()
{
}

class Nes {
private:
  Ram ram{};
  Rom rom{};
  Bus bus{&ram, &rom};
  Cpu cpu{&bus};
public:
  void SetROM(std::string romfile);
  void Start();
};

void Nes::SetROM(std::string romfile)
{
  bus.SetROM(romfile);
}

void Nes::Start()
{
  cpu.Start();
  for (int i=0x8000; i<0x9000; i++) {
    if(i%0x20==0) std::cout << std::endl;
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(static_cast<unsigned char>(bus.Read(i)))
              << ' ';
  }
  std::cout << std::endl;
}

int main()
{
  Nes nes{};
  nes.SetROM("rom/helloworld.nes");
  nes.Start();
  
  return 0;
}
