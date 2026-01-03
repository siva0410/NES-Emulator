#pragma once

#include <array>

class Ram {
private:
  std::array<unsigned char, 0x10000> ram{};
public:
  unsigned char Read(unsigned int addr) const;
  void Write(unsigned int addr, unsigned char data);
};
