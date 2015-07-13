#include "../public/common.h"
#include <iostream>

using micro_ad::utils::Random;
int main(int argc, char *argv[])
{
  Random rand;
  for (int i = 0; i < 10000; i++)
  {
    int count = 0;
    for (int j = 0; j < 50000; j++)
    {
      count += (rand.Next() / (1.0 * RAND_MAX) * 100 < 50.0 ? 1 : 0);
    }
    //if (count < 24000 || count > 26000 )
      std::cout << count << std::endl;
  }
  return 0;
}
