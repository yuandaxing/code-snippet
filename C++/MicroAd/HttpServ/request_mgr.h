#ifndef HTTPSERV_REQUEST_MGR_H_
#define HTTPSERV_REQUEST_MGR_H_

#include <serv.h>
#include <microhttpd.h>

namespace micro_ad
{
namespace utils
{
class RequestMgr: public IHttpRequestProcessor
{
public:
  virtual int Process(const char* url, struct MHD_Connection* conn, std::string& result);
//      virtual ~RequestMgr();

private:
  int Home(struct MHD_Connection* conn, std::string& result);
  int Param(struct MHD_Connection* conn, std::string& result);
  int UpdateParam(const std::string& url, struct MHD_Connection* conn, std::string& result);
  int UpdateParamSucceed(struct MHD_Connection* conn, std::string& result);
  int Monitor(struct MHD_Connection* conn, std::string& result);
};

}
}
#endif
