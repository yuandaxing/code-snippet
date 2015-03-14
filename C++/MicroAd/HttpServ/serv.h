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
    class IHttpRequestProcess
    {
    public:
      virtual int Process(const char* url, struct MHD_Connection* conn, std::string& result) = 0;
      virtual ~IHttpRequestProcess(){};
    };

    class Serv
    {
    public:
      static Serv* Instance();
      int Start(uint16_t port = 8000, std::share_ptr<IHttpRequestProcess> hrp);
      void Stop();

    private:
      static Serv* instance_;
      static pthread_mutex_t lock_;
      struct MHD_Daemon* daemon_;
      std::share_ptr<IHttpRequestProcess> requestPtr_;

      Serv():daemon_(NULL){}
      static int HttpCallback(void* cls, struct MHD_Connection* connection,
                              const char* url, const char* method,
                              const char* version, const char* upload_data,
                              size_t* upload_data_size, void** ptr);
      void HomePage(std::string& content);
      void ParaRepository(std::string& content);
      void ErrorPage(std::string& content);
    };
  }
}


#endif
