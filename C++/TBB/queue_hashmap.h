#ifndef UTILS_QUEUE_H_
#define UTILS_QUEUE_H_
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
/*
 * provide concurrent utilities about non-blocking queue and hash map
 * thin wrap tbb library
 */

namespace concurrent_common
{
template <typename T>
class Queue
{
public:
  Queue(){}
  ~Queue(){}
  void push(const T& item)
  {
    queue_.push(item);
  }
  bool try_pop(T& result)
  {
    return queue_.try_pop(result);
  }
private:
  //no copy
  void operator=(const Queue&);
  Queue(const Queue&);
  tbb::concurrent_queue<T> queue_;
};
template<typename T_, typename V_>
class HashMap
{
public:
  typedef T_ key_t;
  typedef V_ val_t;
  typedef size_t type_size;
  HashMap(): hashmap_() { }
  //if new item, return true, else false
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
  HashMap(const HashMap&);
  void operator=(const HashMap&);
  typedef tbb::concurrent_hash_map<T_, V_> HASHMAP;
  typedef typename HASHMAP::accessor Accessor;
  HASHMAP hashmap_;
};
}
#endif
