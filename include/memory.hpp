#pragma once

#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <cstddef>

using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

class Memory {
private:
  std::vector<uint8_t> memory_;
  
public:
  Memory(std::vector<uint8_t> data);
  size_t Size() const;
  uint8_t Read(uint16_t addr) const;
  void Write(uint16_t addr, uint8_t data);
  uint8_t* Data();
  void Dump() const;
};
