#include <serv.h>
#include <iostream>
#include <functional>

namespace MicroAd
{
namespace Utils
{
Serv* Serv::instance_ = NULL;
pthread_mutex_t Serv::lock_ = PTHREAD_MUTEX_INITIALIZER;
Serv* Serv::Instance()
{
  if(NULL == Serv::instance_)
  {
    pthread_mutex_lock(&Serv::lock_);
    if(NULL == Serv::instance_)
    {
      Serv::instance_ = new Serv();
    }
    pthread_mutex_unlock(&Serv::lock_);
  }
  return Serv::instance_;
}

int Serv::Start(std::shared_ptr<IHttpRequestProcessor> hrp, uint16_t port)
{
  if(NULL == hrp) return -1;

  pthread_mutex_lock(&Serv::lock_);
  if(NULL == daemon_)
  {
    daemon_ = MHD_start_daemon(MHD_USE_POLL_INTERNALLY|MHD_USE_DEBUG, port,
                               NULL, NULL, Serv::HttpCallback,
                               NULL, MHD_OPTION_END);
    requestPtr_ = hrp;
  }
  pthread_mutex_unlock(&Serv::lock_);
  return daemon_ == NULL ? -1 : 0;
}
    
void Serv::Stop()
{
  pthread_mutex_lock(&Serv::lock_);
  if(NULL != daemon_)
  {
    MHD_stop_daemon(daemon_);
    daemon_ = NULL;
  }
  pthread_mutex_lock(&Serv::lock_);
}

int Serv::HttpCallback(void* cls, struct MHD_Connection* connection,
                       const char* url, const char* method,
                       const char* version, const char* upload_data,
                       size_t* upload_data_size, void** ptr)
{
  std::string c;
  std::shared_ptr<IHttpRequestProcessor> requestPtr = Serv::Instance()->requestPtr_;
  std::cout << url;
  if(NULL == requestPtr.get() || 0 != requestPtr->Process(url, connection, c))
  {
    std::cout << "could not get request content" << std::endl;
    return -1;
  }
  MHD_Response* response = MHD_create_response_from_buffer(c.size(), (void *)c.c_str(), MHD_RESPMEM_MUST_COPY);
  if(NULL == response)
  {
    std::cout << "could not create response" << std::endl;
    return -1;
  }

  int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}
}
}
