#include <iostream>
#include <memory>

int main()
{
  std::shared_ptr<int> foo = std::make_shared<int>(10);
  std::cout << "hello" << *foo << std::endl;
}
