#include "nes.hpp"

void Nes::SetRom(std::string romfile)
{
  rom_ = std::make_unique<Rom>(romfile);
  cpubus_.SetRom(rom_.get());
  ppubus_.SetRom(rom_.get());
}

void Nes::Start()
{
  display_.Init();
  
  cpu_.Reset();

  bool running = true;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
	running = false;
    }
    cpu_.Clock();
    ppu_.Clock();
    ppu_.Clock();
    ppu_.Clock();
  }

  display_.Close();

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
