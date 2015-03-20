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
  //empty, release, operator*(), operator->(),
  typedef typename HASHMAP::accessor Accessor;

  bool Find(Accessor& result, const key_t& key);
  bool Erase(const key_t& key);
  bool Erase(const Accessor& accessor);
  bool Size(){ return hashmap_.size(); }

private:
  //no copy
  CacheBase(const CaseBase&);
  void operator=(const CaseBase&);

  typedef tbb::concurrent_hash_map<T_, V_> HASHMAP;
  HASHMAP hashmap_;
}
}
}
#endif
