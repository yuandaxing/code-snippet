#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>

int main(int argc, char *argv[])
{
  while (true)
  {
    unsigned int a = std::rand();
    printf("%u", a);
  }
  return 0;
}
