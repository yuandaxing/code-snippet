#ifndef HTTPSERV_PARAM_REPOSITORY_H_
#define HTTPSERV_PARAM_REPOSITORY_H_
#include <string>
#include <unordered_map>
#include <pthread.h>
namespace MicroAd
{
namespace Utils
{
enum ParamType {BOOL_T, INT32_T, INT64_T, STRING_T };
enum ValSrc {DEFAULT, COMMAND, WEB};
struct ParamInfo
{
  ParamType paramType_;
  std::string name_;
  std::string desc_;
  bool mutable_;
  ValSrc valSrc_;
  void *ptr_;
  ParamInfo(ParamType pt, const char* name, const char* desc, bool alter, void *ptr):
    paramType_(pt), name_(name), desc_(desc), mutable_(alter), valSrc_(DEFAULT), \
    ptr_(ptr) {}
};
class ParamRepository
{
public:
  int32_t CreateInt32(const char* name, const char* val, const char* desc, bool alter, void* ptr)
  {
    ParamInfo pi(INT32_T, name, desc, alter, ptr);
    
  }
  int64_t CreateInt64(const char* name, const char* val, const char* desc, bool alter, void* ptr)
  {
    
  }
  std::string CreateString(const char* name, const char* val, const char* desc, bool alter, void* ptr);
  bool CreateBool(const char* name, const char* val, const char* desc, bool alter, void* ptr);
  bool SetValue(const char* name, const char* val)
  {
    std::string key(name);
    if(paramPool_.find(name) != paramPool_.end())
    {
      ParamInfo& pi = paramPool_[name];
      if(BOOL_T == pi.paramType_)
      {
        bool val = true;
        
      } else if(STRING_T == pi.paramType_)
      {

      }else if(INT32_T == pi.paramType_)
      {

      }else if(INT64_T == pi.paramType_)
      {
      }
      return false;
    }
    return false;
  }

private:
  std::unordered_map<std::string, ParamInfo> paramPool_;
  pthread_mutex_t lock_;
  static ParamRepository instance_;
  
};
}
}
#endif
