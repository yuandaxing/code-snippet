#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

class Random
{
public:
  Random();
  Random(unsigned int seed);
  int Next();
private:
  unsigned int seed_;
};

Random::Random():
  seed_(0)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  seed_ = static_cast<unsigned int>(tv.tv_sec ^ tv.tv_usec);
}
Random::Random(unsigned int seed):
  seed_(seed)
{
}
int Random::Next()
{
  return rand_r(&seed_);
}

int main()
{
  Random rand;
  const int SIZE = 100;
  for (int i = 0; i < 100; i++)
  {
    std::cout << rand.Next() % 100 << std::endl;
  }

  // int distr[100] = {0};
  // for (int i = 0; i < SIZE * 10; i++)
  // {
  //   distr[rand.Next() % SIZE] += 1;
  // }

  // for (int i = 0; i < 100; i++)
  // {
  //   std::cout << distr[i] << std::endl;
  // }

  return 0;
}
