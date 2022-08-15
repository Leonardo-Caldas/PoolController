#include <iostream>
struct eventTri {
  char name[16];
  bool state = false;
};

int main()
{
  eventTri eventsTri[4] = {{"desliga-filtro", false}, {"liga-filtro", true}, {"desliga-luz", false}, {"liga-luz", false}};
  for (int i = 0; i < 4; i++)
  {
    if (eventsTri[i].state)
    {
      std::cout << eventsTri[i].name << "--" << std::endl;
    }
    
  }
}
