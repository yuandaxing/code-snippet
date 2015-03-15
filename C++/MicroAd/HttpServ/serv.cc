#include <serv.h>
#include <iostream>


namespace MicroAd
{
  namespace Utils
  {
    Serv* Serv::instance_ = NULL;
    pthread_mutex_t Serv::lock_ = PTHREAD_MUTEX_INITIALIZER;
    const char* HTMLBEGIN = "<!DOCTYPE html> <html>";
    const char* HTMLCLOSE = "</html>";

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

    int Serv::Start(uint16_t port, std::share_ptr<IHttpRequestProcessor> hrp)
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
      std::cout << "url:" << url << std::endl;
      content << "<head><title>testpage</title></head>";
      
        
      if(strcmp(url, "/param/"))
      {
        
      }
      // {
      // } else if(strcmp(url, "/home"))
      // {
      std::ostringstream content;
      content << HTMLBEGIN;

        "<body><p>hello</p></body>";
      content << HTMLCLOSE;
      std::string c(content.str());
      MHD_Response* response = MHD_create_response_from_buffer(c.size(), (void *)c.c_str(),
                                                 MHD_RESPMEM_MUST_COPY);
      int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
      MHD_destroy_response(response);
      return ret;
    }
  }
}
