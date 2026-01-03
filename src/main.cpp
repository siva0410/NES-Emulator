#include "nes.hpp"

int main()
{
  Nes nes{};
  nes.SetROM("rom/helloworld.nes");
  nes.Start();
  
  return 0;
}
