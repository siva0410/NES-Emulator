#include "display.hpp"

void Display::Write(Point p, RGB color)
{
  buffer_.at((p.x+p.y*256)*3+0) = color.r;
  buffer_.at((p.x+p.y*256)*3+1) = color.g;
  buffer_.at((p.x+p.y*256)*3+2) = color.b;
}

void Display::Init()
{
  width_ = 256;
  height_ = 240;
  scale_ = 3;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    throw std::runtime_error("SDL Error.");
  }

  window_ = SDL_CreateWindow(
			     "NES Emulator",
			     SDL_WINDOWPOS_CENTERED,
			     SDL_WINDOWPOS_CENTERED,
			     width_ * scale_,
			     height_ * scale_,
			     SDL_WINDOW_SHOWN
			     );

  renderer_ = SDL_CreateRenderer(
				 window_,
				 -1,
				 SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
				 );

  texture_ = SDL_CreateTexture(
			       renderer_,
			       SDL_PIXELFORMAT_RGB24,
			       SDL_TEXTUREACCESS_STREAMING,
			       width_,
			       height_
			       );
}

void Display::Update()
{
    SDL_UpdateTexture(texture_, nullptr, buffer_.data(), width_*3);
    
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

void Display::Close()
{
  SDL_DestroyTexture(texture_);
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}
