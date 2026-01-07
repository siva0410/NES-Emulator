#include "nes.hpp"

int main()
{
  Nes nes{"rom/helloworld.nes"};
  nes.Start();
  
  return 0;
}
