#ifndef MICROAD_UTILS_COMMON_H_
#define MICROAD_UTILS_COMMON_H_
#include <pthread.h>
#include <string>
#include <vector>
#include <algorithm>

namespace MicroAd
{
namespace Utils
{
class MutexGuard{
public:
  explicit MutexGuard(pthread_mutex_t *mutex) : mu_(mutex) {
    pthread_mutex_lock(mu_);
  }
  ~MutexGuard() { pthread_mutex_unlock(mu_); }
private:
  pthread_mutex_t *const mu_;
  // No copying allowed
  MutexGuard(const MutexGuard&);
  void operator=(const MutexGuard&);
};

/*
 * string utils
 * to Lower/Upper, split/join by string,
 */
static inline std::string ToLower(const std::string& str)
{
  std::string ret(str);
  std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
  return ret;
}

static inline std::string ToUpper(const std::string& str)
{
  std::string ret(str);
  std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
  return ret;
}

static inline std::string Join(const std::vector<std::string>& strs, const std::string& sep)
{
  std::string ret;
  if(strs.size() < 1)
  {
    return ret;
  }
  ret.append(strs[0]);
  for(std::size_t i = 1; i != strs.size(); i++)
  {
    ret.append(sep);
    ret.append(strs[i]);
  }
  return ret;
}

static inline std::vector<std::string> Split(const std::string& s, const std::string& delim)
{
  std::vector<std::string> ret;
  size_t last = 0;
  size_t index = s.find_first_of(delim,last);
  while (index!=std::string::npos)
  {
    ret.push_back(s.substr(last,index-last));
    last=index+1;
    index=s.find_first_of(delim,last);
  }
  if (index-last>0)
  {
    ret.push_back(s.substr(last,index-last));
  }
  return ret;
}
static inline bool StartWith(const std::string& l, const std::string& r)
{
  if(l.size() < r.size())
  {
    return false;
  } else
  {
    return r.size() == 0 || l.compare(0, r.size(), r) == 0 ;
  }
}

}
}
#endif
