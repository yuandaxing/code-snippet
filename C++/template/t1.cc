#include <iostream>
//type parameter
template<typename T>
T min(const T&a, const T&b)
{
  return a < b ? a : b;
}

int main(int argc, char *argv[])
{
  int a = 1, b = 3;
  std::cout << min(b, a) << std::endl;
  return 0;
}
