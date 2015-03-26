#include <iostream>

template<typename T1, int MAXSIZE>
class Stack
{
private:
  T1 stack_[MAXSIZE];
  int pos;
public:
  void push(T1 v);
};
template<typename T1, int MAXSIZE>
void Stack<T1, MAXSIZE>::push(T1 v)
{
  stack_[pos++] = v;
}

int main(int argc, char *argv[])
{
  Stack<double, 100> s;
  s.push(10.);
  return 0;
}
