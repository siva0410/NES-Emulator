#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include "ram.hpp"
#include "rom.hpp"
#include "bus.hpp"
#include "cpu.hpp"

class Nes {
private:
  Ram ram{};
  Rom rom{};
  Bus bus{&ram, &rom};
  Cpu cpu{&bus};
public:
  void SetROM(std::string romfile);
  void Start();
};
