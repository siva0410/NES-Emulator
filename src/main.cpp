#include "nes.hpp"

int main()
{
  Nes nes{};
  // nes.SetRom("rom/helloworld.nes");
  nes.SetRom("rom/sprite/sprite.nes");
  nes.Start();
  
  return 0;
}
