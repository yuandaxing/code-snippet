/*
 * author: yuandx
 * create: 2015-06-18
 * email: yuandx@mvad.com
 */

#ifndef PUBLIC_THREAD_MANAGER_H_
#define PUBLIC_THREAD_MANAGER_H_
#include <pthread.h>
#include <sys/time.h>
#include <algorithm>
#include <tbb/concurrent_queue.h>
#include <string>
#include <vector>
namespace micro_ad
{
namespace utils
{

using tbb::concurrent_bounded_queue;
typedef void (*task_routine)(void* arg);
using std::string;
using std::vector;

class ThreadManager
{
public:
  ThreadManager(int thread_num, const string& manager_name,
                int task_capacity = 100000);
  void Start();
  void Stop();
  void AddTask(task_routine routine, void* arg);

private:
  void Worker();
  static void* WorkerWrapper(void*);

private:
  //no copy
  ThreadManager(const ThreadManager&);
  void operator=(const ThreadManager&);

  struct Task
  {
    void* arg_;
    task_routine routine_;
    Task();
    Task(void* arg, task_routine routine);
  };

private:
  bool running_;
  const int thread_num_;
  concurrent_bounded_queue<Task> tasks_;
  vector<pthread_t> thread_ids_;
  string name_;
};

inline ThreadManager::ThreadManager(int thread_num, const string& manager_name, int task_capacity) :
  running_(false), thread_num_(thread_num)
{
  name_ = manager_name;
  tasks_.set_capacity(task_capacity);
}

inline void ThreadManager::Start()
{
  running_ = true;
  for (int i = 0; i != thread_num_; ++i)
  {
    pthread_t id;
    pthread_create(&id, NULL, ThreadManager::WorkerWrapper, this);
    thread_ids_.push_back(id);
  }
}

inline void ThreadManager::Stop()
{
  running_ = false;
}

inline void ThreadManager::AddTask(task_routine routine, void* arg)
{
  tasks_.push(Task(arg, routine));
}

inline void ThreadManager::Worker()
{
  while (running_)
  {
    Task t;
    tasks_.pop(t);
    if (t.routine_)
    {
      t.routine_(t.arg_);
    }
  }
}

inline void* ThreadManager::WorkerWrapper(void* arg)
{
  ThreadManager* tm = static_cast<ThreadManager*>(arg);
  tm->Worker();
}
inline ThreadManager::Task::Task():
  arg_(NULL), routine_(NULL)
{
}
inline ThreadManager::Task::Task(void* arg, task_routine routine):
  arg_(arg), routine_(routine)
{
}

}
}
#endif // PUBLIC_THREAD_MANAGER_H_
