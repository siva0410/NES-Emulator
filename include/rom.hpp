#pragma once

#include <fstream>
#include <array>

class Rom {
private:
  std::array<unsigned char, 0x8000> rom{};
public:
  void Set(std::string romfile);
  unsigned char Read(unsigned int addr) const;
};
