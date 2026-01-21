#include "display.hpp"

void Display::Write(Point p, RGB color)
{
  display_.at(p.x+p.y*256) = color;
}

RGB Display::Read(Point p) const
{
  return display_.at(p.x+p.y*256);
}

// void Display::Write(Point p, uint8_t data)
// {
//   display_.at(p.x+p.y*256) = data;
// }

// void Display::Dump() const
// {
//   for(int y=0; y<240; y++) {
//     for(int x=0; x<256; x++){
//       std::cout << std::hex << std::setw(2) << std::setfill('0')
// 		<< static_cast<unsigned int>(display_.at(x+y*256))
// 		<< "";
//     }
//     std::cout << std::endl;
//   }
// }
