#include <iostream>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>
const int NUM = 1000000;
class ReadGuard {
 public:
  explicit ReadGuard(pthread_rwlock_t *rw) : rw_lock_(rw)
  {
    pthread_rwlock_rdlock(rw_lock_);
  }
  ~ReadGuard() { pthread_rwlock_unlock(rw_lock_); }
 private:
  pthread_rwlock_t *const rw_lock_;
  // No copying allowed
  ReadGuard(const ReadGuard&);
  void operator=(const ReadGuard&);
};

int main(int argc, char *argv[])
{
  pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;
  int64_t sum = 0;
  timeval start, end;

  gettimeofday(&start, NULL);
  for (int i = 0; i < NUM; i++)
  {
    {
       ReadGuard rg(&rw_lock);
      sum += i;
    }
  }
  gettimeofday(&end, NULL);
  double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);

  std::cout << "sum: "<< sum << std::endl;
  std::cout << "time :" << us << std::endl;
  return 0;
}
