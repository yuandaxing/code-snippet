#include <serv.h>
#include <stdio.h>
#include <iostream>
using namespace MicroAd::Utils;
int main(int argc, char *argv[])
{
  Serv* serv = Serv::Instance();
  if(0 != serv->Start(8000))
  {
    std::cout << "could not start server" << std::endl;
  }
  getchar();
  serv->Stop();
  return 0;
}
