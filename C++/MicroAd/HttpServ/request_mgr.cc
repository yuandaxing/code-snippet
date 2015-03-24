#include <request_mgr.h>
#include <param_repository.h>
namespace MicroAd
{
namespace Utils
{
int RequestMgr::Process(const char* url, struct MHD_Connection* conn, std::string& result)
{
  if(NULL == url || NULL == conn)
  {
    return -1;
  }
  std::string surl(url);
  if(surl == "/")
  {
    return Home(conn, result);
  }
  if(surl == "/param")
  {
    return Param(conn, result);
  }
  if(surl == "/param/update")
  {
    return UpdateParam(conn, result);
  }
  if(surl == "/monitor")
  {
    return Monitor(conn, result);
  }
  return -1;
}


#define HTMLBEGIN(title)                                                \
  "<!DOCTYPE html>"                                                     \
  "<html>"                                                              \
  "<head>"                                                              \
  "<title>" title "</title>"                                            \
  "<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\"/>" \
  "</head>"                                                             \
  "<body>"
#define HTMLEND  "</body></html>"
int RequestMgr::Home(struct MHD_Connection* conn, std::string& result)
{
  result.append(HTMLBEGIN("HOMEPAGE"));
  const std::string content[] =
    {"<p>this is a test for home page</p>",
     "<a href=\"monitor\">Monitor</a><br>",
     "<a href=\"param\">Param</a><br>",
     "<a href=\"notknow\">Notknow</a><br>"
    };
  for(std::size_t i = 0; i != sizeof(content)/sizeof(*content); i++)
  {
    result.append(content[i]);
  }
  result.append(HTMLEND);
  return 0;
}

int RequestMgr::Monitor(struct MHD_Connection* conn, std::string& result)
{
  result.append(HTMLBEGIN("MONITOR"));
  result.append("<p>p:1,2,3,4,5</p>");
  result.append(HTMLEND);
  return 0;
}
int RequestMgr::Param(struct MHD_Connection* conn, std::string& result)
{
  result.append(HTMLBEGIN("PARAM"));
  std::string content;
  int ret = 0;
  if(ParamRepository::Instance()->TableInfo(content))
  {
    result += content;
  } else
  {
    result += "<p>Error</p>";
    ret = -1;
  }
  result.append(HTMLEND);
  return ret;
}
int RequestMgr::UpdateParam(struct MHD_Connection* conn, std::string& result)
{
  //Todo
  return 0;
}
int RequestMgr::UpdateParamSucceed(struct MHD_Connection* conn, std::string& result)
{
  //Todo
  return 0;
}
}
}
