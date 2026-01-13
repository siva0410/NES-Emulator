#include "nes.hpp"
#include <memory>

void Nes::SetRom(std::string romfile)
{
  rom_ = std::make_unique<Rom>(romfile);
  cpubus_.SetRom(rom_.get());
}

void Nes::Start()
{
  cpu_.Reset();

  for(int i=0; i<1000; i++){
    cpu_.Tick();
  }

  std::cout << "-----ROM DUMP-----" << std:: endl;
  rom_->Dump();
  std::cout << std::endl;
  
  std::cout << "-----WRAM DUMP-----" << std:: endl;
  wram_.Dump();
  std::cout << std::endl;
    
  std::cout << "-----VRAM DUMP-----" << std:: endl;
  vram_.Dump();
  std::cout << std::endl;
}
