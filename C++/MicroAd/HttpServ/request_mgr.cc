#include <iostream>
#include <request_mgr.h>
#include <param_repository.h>
#include <ad_common.h>
#include <sstream>
namespace MicroAd
{
namespace Utils
{

int RequestMgr::Process(const char* url, struct MHD_Connection* conn, std::string& result)
{
  std::cout << std::endl << "process" << std::endl;
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
  if(StartWith(surl, "/param/update"))
  {
    return UpdateParam(surl, conn, result);
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
  }
  else
  {
    result += "<p>Error</p>";
    ret = -1;
  }
  result.append(HTMLEND);
  return ret;
}

int RequestMgr::UpdateParam(const std::string& url, struct MHD_Connection* conn, std::string& result)
{
  //Todo

  std::vector<std::string> splits = Split(url, "/");
  std::cout << "update param";
  if(splits.size() < 3)
  {
    return -1;
  }
  else
  {
    std::string key = splits[3];
    std::string setOps;
    const char* val = MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND, key.c_str());
    if (NULL != val)
    {
      if(!ParamRepository::Instance()->SetValue(key, val, ValSrc::WEB))
        setOps = "<p>Set operation failed</p>";
      else
        setOps = "<p>Set operation success</p>";
    }
    ParamInfo pi = ParamRepository::Instance()->Get(key);

    std::ostringstream content;
    result.append(HTMLBEGIN("UPDATE"));
    result.append(setOps);
    content << "<p>name: " << pi.name_ <<  " type: " << ParamTypeString(pi.paramType_) << "</p>";
    //    content << "<form action=\"/param/update/" << key <<"\" method=\"post\">" ;
    content << "<form name=\"input\" action=\"/param/update/" << key <<"/#\" method=\"get\">" ;
    content << "<p>" << pi.name_ << "</p>";
    content << "<input type=\"text\" name=\"" << key <<"\" />" ;
    content << "<input type=\"submit\" value=\"Submit\"/>";
    content << "</form>";
    result.append(content.str());
  }
  return 0;
}
int RequestMgr::UpdateParamSucceed(struct MHD_Connection* conn, std::string& result)
{
  //Todo
  return 0;
}

}
}
