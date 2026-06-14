#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

using std::int8_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

struct PadRegister {
  uint8_t button;
};

enum class Button {
  A = 0,
  B = 1,
  Select = 2,
  Start = 3,
  Up = 4,
  Down = 5,
  Left = 6,
  Right = 7,
};

class Controller {
private:
  uint8_t strobe_{};
  uint8_t buttons_{};
  uint8_t shift_{};
  PadRegister regs_{};
public:
  Controller();
  void SetButton(Button button, bool pressed);
  void Handler();
  uint8_t Read();
  void Write(uint8_t data);
};
