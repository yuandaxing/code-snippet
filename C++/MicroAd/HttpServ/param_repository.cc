#include <param_repository.h>
namespace MicroAd
{
namespace Utils
{
pthread_mutex_t ParamRepository::lock_ = PTHREAD_MUTEX_INITIALIZER;
ParamRepository* ParamRepository::instance_ = NULL;
int32_t ParamRepository::CreateInt32(const char* name, const char* val, 
                                     const char* desc, bool alter, void* ptr)
{
  ParamInfo pi(INT32_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return atoi(val);
}
int64_t ParamRepository::CreateInt64(const char* name, const char* val, const char* desc,
                                     bool alter, void* ptr)
{
  ParamInfo pi(INT64_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return atoll(val);
}
std::string ParamRepository::CreateString(const char* name, const char* val,
                                          const char* desc, bool alter, void* ptr)
{
  ParamInfo pi(STRING_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return std::string(val);
}
bool ParamRepository::CreateBool(const char* name, const char* val, const char* desc, bool alter, void* ptr)
{
  ParamInfo pi(BOOL_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return string(val) == string("true");
}

bool ParamRepository::SetValue(const char* name, const char* val, const ValSrc vs)
{
  std::string key(name);
  if(paramPool_.find(name) != paramPool_.end())
  {
    ParamInfo& pi = paramPool_[name];
    if(!pi.mutable_) return false;
    if(BOOL_T == pi.paramType_)
    {
      string boolStr(tolower(val));
      bool* ptr = static_cast<bool*>(pi.ptr_);
      if(string("true") == boolStr)
      {
        *ptr = true;
      }
      else if(string("false") == boolStr)
      {
        *ptr = false;
      }
      else
      {
        return false;
      }
    } else if(STRING_T == pi.paramType_)
    {
      std::string* ptr = static_cast<std::string*>(pi.ptr_);
      *ptr = val;
    }else if(INT32_T == pi.paramType_)
    {
      int32_t* ptr = static_cast<int32_t*>(pi.ptr_);
      *ptr = atoi(val);
    }else if(INT64_T == pi.paramType_)
    {
      int64_t* ptr = static_cast<int64_t*>(pi.ptr_);
      *ptr = atoll(val);
    }
    pi.valSrc_ = vs;
    return true;
  }
  return false;
}
ParamRepository* ParamRepository::Instance()
{
  if(NULL == instance_)
  {
    pthread_mutex_lock(&lock_);
    if(NULL == instance_)
    {
      instance = new ParamRepository();
    }
    pthread_mutex_unlock(&lock_);
  }
  return instance_;
}
}
}
}
