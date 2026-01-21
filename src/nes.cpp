#include "nes.hpp"
#include <memory>

void Nes::SetRom(std::string romfile)
{
  rom_ = std::make_unique<Rom>(romfile);
  cpubus_.SetRom(rom_.get());
  ppubus_.SetRom(rom_.get());
}

bool Nes::WindowStart()
{
  int W = 256;
  int H = 240;
  int SCALE = 3;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
					"SDL2 256x240 RGB",
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					W * SCALE,
					H * SCALE,
					SDL_WINDOW_SHOWN
					);

  SDL_Renderer* renderer = SDL_CreateRenderer(
					      window,
					      -1,
					      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
					      );

  // 256x240 RGB テクスチャ
  SDL_Texture* texture = SDL_CreateTexture(
					   renderer,
					   SDL_PIXELFORMAT_RGB24,          // ← RGB 配列
					   SDL_TEXTUREACCESS_STREAMING,
					   W, H
					   );

  // ボケ防止（超重要）
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

  // フレームバッファ（RGB888）
  uint8_t framebuffer[W * H * 3];

  bool running = true;
  SDL_Event e;

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
	running = false;
    }

    // -----------------------------
    // テスト描画（RGB 配列を埋める）
    // -----------------------------
    for (uint32_t y = 0; y < H; ++y) {
      for (uint32_t x = 0; x < W; ++x) {
	Point p{x,y};
	uint32_t i = (x+W*y)*3;
	framebuffer[i + 0] = display_.Read(p).r;
	framebuffer[i + 1] = display_.Read(p).g;
	framebuffer[i + 2] = display_.Read(p).b;
      }
    }

    // RGB 配列 → テクスチャへ転送
    SDL_UpdateTexture(
		      texture,
		      nullptr,
		      framebuffer,
		      W * 3          // pitch = 1行あたりのバイト数
		      );

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return true;
}

void Nes::Start()
{
  cpu_.Reset();

  for(int i=0; i<1000; i++){
    cpu_.Tick();
  }
  
  ppu_.Tick();

  WindowStart();

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
