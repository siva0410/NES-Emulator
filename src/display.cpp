#include "display.hpp"

void Display::Write(Point p, RGB color)
{
  display_.at(p.x+p.y*256) = color.r<<4 | color.g<<2 | color.b;
}

void Display::Dump() const
{
  for(int i=0; i<240; i++) {
    for(int j=0; j<256; j++){
      std::cout << std::hex << std::setw(2) << std::setfill('0')
		<< static_cast<unsigned int>(display_.at(i*256+j))
		<< "";
    }
    std::cout << std::endl;
  }
}
