#include <iostream>
template <typename T1, typename T2>
class A
{
private:
  T1 i;
  T2 j;
public:
  A(T1 a, T2 b);
  void show();
};
template<typename T1, typename T2>
A<T1, T2>::A(T1 a, T2 b): i(a), j(b){}


template<typename T1, typename T2>
void A<T1, T2>::show()
{
  std::cout << "i = " << i << std::endl << "j = " << j << std::endl;
}

int main(int argc, char *argv[])
{
  A<int, double> a(1, 3.);
  a.show();
  return 0;
}
