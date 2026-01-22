#pragma once

#include <cstdint>
#include <array>
#include <SDL2/SDL.h>

#include "ppubus.hpp"


using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct Point {
  uint32_t x;
  uint32_t y;
};

class Display {
private:
  int width_{};
  int height_{};
  int scale_{};
  SDL_Window* window_{};
  SDL_Renderer* renderer_{};
  SDL_Texture* texture_{};
  std::array<uint8_t,256*240*3> buffer_{};
public:
  void Init();
  void Open();
  void Update();
  void Close();
  void Write(Point p, RGB color);
};
