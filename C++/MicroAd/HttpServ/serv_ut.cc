#include <serv.h>
#include <request_mgr.h>
#include <stdio.h>
#include <iostream>
using namespace MicroAd::Utils;
int main(int argc, char *argv[])
{
  std::shared_ptr<IHttpRequestProcessor> hrp(new RequestMgr());
  Serv* serv = Serv::Instance();
  if(0 != serv->Start(hrp, 8000))
  {
    std::cout << "could not start server" << std::endl;
  }
  getchar();
  serv->Stop();
  return 0;
}
