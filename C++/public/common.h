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
#include <stdint.h>
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


inline std::string ToLower(const std::string& str)
{
  std::string ret(str);
  std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
  return ret;
}

inline std::string ToUpper(const std::string& str)
{
  std::string ret(str);
  std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
  return ret;
}

inline std::string Join(const std::vector<std::string>& strs, const std::string& sep)
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

inline std::vector<std::string> Split(const std::string& s, const std::string& delim)
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

inline bool StartWith(const std::string& l, const std::string& r)
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

inline Random::Random():
  seed_(0)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  seed_ = static_cast<unsigned int>(tv.tv_sec ^ tv.tv_usec);
}
inline Random::Random(unsigned int seed):
  seed_(seed)
{
}
inline int Random::Next()
{
  return rand_r(&seed_);
}

inline int64_t CurrentUSeconds()
{
  struct timeval cur;
  gettimeofday(&cur, NULL);
  return cur.tv_sec *1000000 + cur.tv_usec;
}

template <typename Iterator>
void LockFreeShuffle(Iterator begin, Iterator end)
{
  Random rand_;
  for (std::ptrdiff_t size = end - begin; size != 0; size--)
  {
    std::swap(*(begin + size - 1), *(begin + rand_.Next() % size));
  }
}

}
}
#endif // PUBLIC_COMMON_H_
