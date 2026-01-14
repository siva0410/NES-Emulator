#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

#include "ram.hpp"
#include "rom.hpp"
#include "cpu.hpp"
#include "cpubus.hpp"
#include "ppu.hpp"
#include "ppubus.hpp"
#include "display.hpp"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Nes {
private:
  std::unique_ptr<Rom> rom_{};
  Display display_{};
  Ram wram_{0x0800};
  Ram vram_{0x0800};
  Ram palletram_{0x0020};
  Cpu cpu_{cpubus_};
  Ppu ppu_{ppubus_, palletram_, display_};
  CpuBus cpubus_{ppu_, wram_};
  PpuBus ppubus_{vram_, palletram_};
public:
  void SetRom(std::string romfile);
  void Start();
};
