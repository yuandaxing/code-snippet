#include <param_repository.h>
#include <ad_common.h>
#include <algorithm>
#include <sstream>
namespace MicroAd
{
namespace Utils
{

pthread_mutex_t ParamRepository::lock_ = PTHREAD_MUTEX_INITIALIZER;
ParamRepository* ParamRepository::instance_ = NULL;

int32_t ParamRepository::CreateInt32(const char* name, const char* val, 
                                     const char* desc, bool alter, void* ptr)
{
  MutexGuard guard(&ParamRepository::lock_);
  ParamInfo pi(INT32_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return atoi(val);
}
int64_t ParamRepository::CreateInt64(const char* name, const char* val, const char* desc,
                                     bool alter, void* ptr)
{
  MutexGuard guard(&ParamRepository::lock_);
  ParamInfo pi(INT64_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return atoll(val);
}
std::string ParamRepository::CreateString(const char* name, const char* val,
                                          const char* desc, bool alter, void* ptr)
{
  MutexGuard guard(&ParamRepository::lock_);
  ParamInfo pi(STRING_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return std::string(val);
}
bool ParamRepository::CreateBool(const char* name, const char* val, const char* desc,
                                 bool alter, void* ptr)
{
  MutexGuard guard(&ParamRepository::lock_);
  ParamInfo pi(BOOL_T, name, desc, alter, ptr);
  std::string key(name);
  paramPool_[key] = pi;
  return std::string(val) == std::string("true");
}

bool ParamRepository::SetValue(const std::string& name, const std::string& val, const ValSrc vs)
{
  std::string key(name);
  MutexGuard guard(&ParamRepository::lock_);
  if(paramPool_.find(name) != paramPool_.end())
  {
    ParamInfo& pi = paramPool_[name];
    if(!pi.mutable_) return false;
    if(BOOL_T == pi.paramType_)
    {
      std::string boolStr(val);
      std::transform(boolStr.begin(), boolStr.end(), boolStr.begin(), ::tolower);
      bool* ptr = static_cast<bool*>(pi.ptr_);
      if(std::string("true") == boolStr)
      {
        *ptr = true;
      }
      else if(std::string("false") == boolStr)
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
      *ptr = atoi(val.c_str());
    }else if(INT64_T == pi.paramType_)
    {
      int64_t* ptr = static_cast<int64_t*>(pi.ptr_);
      *ptr = atoll(val.c_str());
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
      instance_ = new ParamRepository();
    }
    pthread_mutex_unlock(&lock_);
  }
  return instance_;
}
bool ParamRepository::TableInfo(std::string& info)
{
  typedef std::unordered_map<std::string, ParamInfo> PoolType;
  std::ostringstream ss;
  ss << "<table border=\"1\">" << std::endl;
  ss << "<tr><th>Name</th><th>Desc</th><th>Mutable</th><th>Type</th><th>Value</th></tr>"<< std::endl;
  MutexGuard guard(&ParamRepository::lock_);
  for(PoolType::iterator it = paramPool_.begin(); it != paramPool_.end(); ++it)
  {
    ParamInfo& pi = it->second;
    ss << "<tr>";
    if(pi.mutable_)
    {
      ss << "<td><a href=\"/param/update/" << pi.name_ << "\">" << pi.name_ << "</a></td>";
    } else
    {
      ss << "<td>" << pi.name_ << "</td>";
    }
    ss << "<td>" << pi.desc_ << "</td>";
    ss << "<td>" << (pi.mutable_ ? "true" : "false") << "</td>";
    if(pi.paramType_ == BOOL_T)
    {
      ss << "<td>Bool</td><td>" << (*static_cast<bool*>(pi.ptr_) ? "true" : "False") \
         << "</td>" << std::endl;
    } else if(pi.paramType_ == INT32_T)
    {
      ss << "<td>Int32</td><td>" << *static_cast<int32_t*>(pi.ptr_) << "</td>";
    } else if(pi.paramType_ == INT64_T)
    {
      ss << "<td>Int64</td><td>" << *static_cast<int64_t*>(pi.ptr_) << "</td>";
    } else if(pi.paramType_ == STRING_T)
    {
      ss << "<td>String</td><td>" << *static_cast<std::string*>(pi.ptr_) << "</td>";
    }else
    {
      return false;
    }
    ss << "</tr>";
  }
  ss << "<table>";
  info.append(ss.str());
  return true;
}
}
}

