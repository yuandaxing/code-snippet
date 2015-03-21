#ifndef HTTPSERV_SERV_H_
#define HTTPSERV_SERV_H_

#include <memory>
#include <microhttpd.h>
#include <pthread.h>
#include <string>
#include <sys/types.h>

namespace MicroAd
{
namespace Utils
{
class IHttpRequestProcessor
{
public:
  virtual int Process(const char* url, struct MHD_Connection* conn, std::string& result) = 0;
  virtual ~IHttpRequestProcessor(){}
};

class Serv
{
public:
  static Serv* Instance();
  int Start(std::shared_ptr<IHttpRequestProcessor> hrp, uint16_t port = 8000);
  void Stop();

private:
  static Serv* instance_;
  static pthread_mutex_t lock_;
  struct MHD_Daemon* daemon_;
  std::shared_ptr<IHttpRequestProcessor> requestPtr_;

  Serv():daemon_(NULL){}
  static int HttpCallback(void* cls, struct MHD_Connection* connection,
                          const char* url, const char* method,
                          const char* version, const char* upload_data,
                          size_t* upload_data_size, void** ptr);
};
}
}
#endif
