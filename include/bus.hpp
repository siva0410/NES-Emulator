#pragma once

#include <string>

#include "ram.hpp"
#include "rom.hpp"

class Bus {
private:
  Ram* ram_;
  Rom* rom_;
public:
  Bus(Ram* ram, Rom* rom);
  unsigned char Read(unsigned int addr) const;
  void Write(unsigned int addr, unsigned char data);
  void SetROM(std::string romfile);
};
