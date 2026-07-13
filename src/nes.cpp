#include "nes.hpp"
#include "SDL_keycode.h"
#include <chrono>
#include <thread>

namespace {
  constexpr auto kClockSleep = std::chrono::microseconds{400};
}

void Nes::SetRom(std::string romfile)
{
  rom_ = std::make_unique<Rom>(romfile);
  cpubus_.SetRom(rom_.get());
  ppubus_.SetRom(rom_.get());
}

void Nes::KeyboardHandler(const SDL_Event& event)
{
  if (event.type != SDL_KEYDOWN and event.type != SDL_KEYUP) {
    return;
  }

  bool pressed = (event.type == SDL_KEYDOWN);

  switch(event.key.keysym.sym) {
  case SDLK_SPACE:
    controller1_.SetButton(Button::A, pressed);
    break;

  case SDLK_LSHIFT:
    controller1_.SetButton(Button::B, pressed);
    break;

  case SDLK_RSHIFT:
    controller1_.SetButton(Button::Select, pressed);
    break;

  case SDLK_RETURN:
    controller1_.SetButton(Button::Start, pressed);
    break;
    
  case SDLK_UP:
    controller1_.SetButton(Button::Up, pressed);
    break;

  case SDLK_DOWN:
    controller1_.SetButton(Button::Down, pressed);
    break;

  case SDLK_LEFT:
    controller1_.SetButton(Button::Left, pressed);
    break;

  case SDLK_RIGHT:
    controller1_.SetButton(Button::Right, pressed);
    break;

  default:
    break;
  }
}

void Nes::Start()
{
  display_.Init();
  
  cpu_.Reset();

  bool running = true;
  uint32_t clock{};
  SDL_Event event{};
  while (running) {
    if (clock == 0x3FF) {
      while (SDL_PollEvent(&event)) {
	if (event.type == SDL_QUIT) {
	  running = false;
	}
	KeyboardHandler(event);
      }
      std::this_thread::sleep_for(kClockSleep);
      clock = 0;
    }
    
    cpu_.Clock();
    for(uint8_t i=0; i<3; i++) {
      ppu_.Clock();
      if(ppu_.IsNmiEnable() and ppu_.ConsumeNmi()) {
	cpu_.RequestNmi();
      }
    }
    
    clock++;
  }

  display_.Close();

  std::cout << "-----ROM DUMP-----" << std:: endl;
  rom_->Dump();
  std::cout << std::endl;

  std::cout << "-----Cartridge RAM DUMP-----" << std:: endl;
  rom_->RamDump();
  std::cout << std::endl;
  
  std::cout << "-----WRAM DUMP-----" << std:: endl;
  wram_.Dump();
  std::cout << std::endl;
    
  std::cout << "-----VRAM DUMP-----" << std:: endl;
  vram_.Dump();
  std::cout << std::endl;

  std::cout << "-----PALLET DUMP-----" << std:: endl;
  palletram_.Dump();
  std::cout << std::endl;
}
