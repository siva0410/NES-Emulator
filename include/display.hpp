#pragma once

#include <cstdint>
#include <array>

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
  // std::array<uint8_t,256*240> display_{};
  std::array<RGB,256*240> display_{};
public:
  void Write(Point p, RGB color);
  RGB Read(Point p) const;
  void Dump() const;
};
