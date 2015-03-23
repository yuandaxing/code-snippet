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
  typedef T_ key_t;
  typedef V_ val_t;
  typedef size_t type_size;
  CacheBase(): hashmap_() { }

  bool Set(const key_t& key, const val_t& val)
  {
    Accessor accessor;
    bool newItem = hashmap_.insert(accessor, key);
    accessor->second = val;
    return newItem;
  }

  bool Get(const key_t& key, val_t& val) const
  {
    Accessor accessor;
    if(hashmap_.find(accessor, key))
    {
      val = accessor->second;
      return true;
    }
    return false;
  }

  bool Erase(const key_t& key)
  {
    return hashmap_.erase(key);
  }
  //insert all the way, true is new, else old

  type_size Size() const { return hashmap_.size(); }

private:
  //no copy
  CacheBase(const CacheBase&);
  void operator=(const CacheBase&);
  typedef tbb::concurrent_hash_map<T_, V_> HASHMAP;
  typedef typename HASHMAP::accessor Accessor;
  HASHMAP hashmap_;
};
}
}
#endif
