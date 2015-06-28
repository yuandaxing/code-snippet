/*
 * author: yuandx
 * create: 2015-06-18
 * email: yuandx@mvad.com
 */

#ifndef PUBLIC_COMMON_H_
#define PUBLIC_COMMON_H_
#include <pthread.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <cstdint>
namespace micro_ad
{
namespace utils
{

class MutexGuard
{
public:
  explicit MutexGuard(pthread_mutex_t *mutex) : mu_(mutex)
  {
    pthread_mutex_lock(mu_);
  }
  ~MutexGuard()
  {
    pthread_mutex_unlock(mu_);
  }
private:
  // No copying allowed
  MutexGuard(const MutexGuard&);
  void operator=(const MutexGuard&);
  pthread_mutex_t *const mu_;
};

class ScopeSpinlock
{
public:
  ScopeSpinlock(pthread_spinlock_t * lock)
  {
    lock_ = lock;
    pthread_spin_lock(lock_);
  }

  ~ScopeSpinlock()
  {
    pthread_spin_unlock(lock_);
  }

private:
  pthread_spinlock_t * lock_;
};

class ScopeRwlock
{
public:
  ScopeRwlock(pthread_rwlock_t * lock, bool is_readlock)
  {
    lock_ = lock;
    if (is_readlock)
    {
      pthread_rwlock_rdlock(lock_);
    }
    else
    {
      pthread_rwlock_wrlock(lock_);
    }
  }

  ~ScopeRwlock()
  {
    pthread_rwlock_unlock(lock_);
  }

private:
  pthread_rwlock_t * lock_;
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
  size_t last = 0, index = s.find_first_of(delim, last);

  while (index != std::string::npos)
  {
    ret.push_back(s.substr(last, index - last));
    last = index + 1;
    index = s.find_first_of(delim, last);
  }

  if (index - last > 0)
  {
    ret.push_back(s.substr(last, index - last));
  }
  return ret;
}

static inline bool StartWith(const std::string& l, const std::string& r)
{
  return l.compare(0, r.size(), r) == 0;
}

class Random
{
public:
  Random();
  Random(unsigned int seed);
  int Next();
private:
  unsigned int seed_;
};

Random::Random():
  seed_(0)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  seed_ = static_cast<unsigned int>(tv.tv_sec ^ tv.tv_usec);
}
Random::Random(unsigned int seed):
  seed_(seed)
{
}
int Random::Next()
{
  return rand_r(&seed_);
}

int64_t CurrentUSeconds()
{
  struct timeval cur;
  gettimeofday(&cur, NULL);
  return cur.tv_sec *1000000 + cur.tv_usec;
}

}
}
#endif // PUBLIC_COMMON_H_
