#include "nes.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: nes <romfile path>\n";
    return 1;
  }
  
  std::string romPath{argv[1]};
  
  Nes nes{};
  
  nes.SetRom(romPath);  
  nes.Start();
  
  return 0;
}
