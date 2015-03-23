#include <pthread.h>
namespace MicroAd
{
namespace Utils
{
class MutexGuard {
public:
  explicit MutexGuard(pthread_mutext_t *mutex) : mu_(mutex) {
    pthread_mutex_lock(mu_);
  }
  ~MutexGuard() { pthread_mutex_unlock(mu_); }
private:
  port::Mutex *const mu_;
  // No copying allowed
  MutexGuard(const MutexGuard&);
  void operator=(const MutexGuard&);
};
}
}
