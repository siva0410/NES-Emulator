#pragma once

#include "bus.hpp"

class Cpu {
private:
  Bus* bus_;
public:
  Cpu(Bus* bus);
  void Start();
};
