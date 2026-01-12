#include "nes.hpp"

Nes::Nes(std::string romfile)
  : rom_(romfile)
{
}

void Nes::Start()
{
  cpu_.Reset();

  for(int i=0; i<1000; i++){
    cpu_.Tick();
  }
  
  for (int i=0x0000; i<0x0800; i++) {
    if(i%0x20==0) std::cout << std::endl;
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(cpubus_.Read(i))
              << ' ';
  }
  std::cout << std::endl;
}
