#include <controller.hpp>

Controller::Controller()
{
}

void Controller::Handler()
{
  
}

void Controller::SetButton(Button button, bool pressed)
{
  uint8_t buttonBit = 0b1 << static_cast<uint8_t>(button);

  if (pressed) {
    buttons_ = buttons_ | buttonBit;
  } else {
    buttons_ &= buttons_ & ~buttonBit;
  }
}

uint8_t Controller::Read()
{
  uint8_t res;
  if(strobe_) {
    return buttons_ & 0b1;
  }
  res = shift_&0b1;
  shift_ = shift_>>1;
  shift_ = shift_ | 0b10000000;
  return res;
}

void Controller::Write(uint8_t data)
{
  strobe_ = data & 0b1;
  if(strobe_) {
    shift_ = buttons_;
  }
}
