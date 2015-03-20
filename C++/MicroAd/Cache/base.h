#ifndef MICROAD_CACHE_BASE_H_
#define MICROAD_CACHE_BASE_H_
#include "tbb/concurrent_hash_map.h"

namespace MicroAd
{
namespace Utils
{
template<typename T_, typename V_>
class CacheBase
{
public:
  typedef typename T_ key_t;
  typedef typename v_ val_t;
  typedef size_t type_size;

  bool Get(const key_t& key, val_t& val)
  {
    Accessor accessor;
    if(hashmap_.find(accessor, key))
    {
      val = accessor->second;
      return true;
    }
    return false;
  }

  bool Erase(const key_t& key);
  {
    hashmap_.erase(key);
  }
  bool Set(const key_t& key, const val_t val)

  bool Size(){ return hashmap_.size(); }

private:
  //no copy
  CacheBase(const CaseBase&);
  void operator=(const CaseBase&);
  typedef tbb::concurrent_hash_map<T_, V_> HASHMAP;
  typedef typename HASHMAP::accessor Accessor;
  HASHMAP hashmap_;
}
}
}
#endif
