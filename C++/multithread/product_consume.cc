#include <unistd.h>
#include <pthread.h>
#include <list>
#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <time.h>
#include <set>
#include <stdlib.h>
#include <algorithm>

using std::cout;
using std::set;
using std::list;
using std::string;
using std::vector;
typedef void (*task_routine)(void* arg);

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



class ThreadManager
{
public:
  ThreadManager(int thread_num, const string& manager_name);
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

  bool running_;
  const int thread_num_;
  list<Task> tasks_;
  vector<pthread_t> thread_ids_;
  pthread_mutex_t mutex_;
  string name_;
};

ThreadManager::ThreadManager(int thread_num, const string& manager_name) :
  running_(false), thread_num_(thread_num), mutex_(PTHREAD_MUTEX_INITIALIZER)
{
  name_ = manager_name;
}

void ThreadManager::Start()
{
  running_ = true;
  for (int i = 0; i != thread_num_; ++i)
  {
    pthread_t id;
    pthread_create(&id, NULL, ThreadManager::WorkerWrapper, this);
    thread_ids_.push_back(id);
  }
}

void ThreadManager::Stop()
{
  running_ = false;
}

void ThreadManager::AddTask(task_routine routine, void* arg)
{
  MutexGuard m(&mutex_);
  tasks_.push_back(Task(arg, routine));
}

void ThreadManager::Worker()
{
  while (running_)
  {
    Task t;
    {
      MutexGuard m(&mutex_);
      if (!tasks_.empty())
      {
        t = tasks_.front();
        tasks_.pop_front();
      }
    }
    if (t.routine_)
    {
      t.routine_(t.arg_);
    }
  }
}

void* ThreadManager::WorkerWrapper(void* arg)
{
  ThreadManager* tm = static_cast<ThreadManager*>(arg);
  tm->Worker();
}
ThreadManager::Task::Task():
  arg_(NULL), routine_(NULL)
{
}
ThreadManager::Task::Task(void* arg, task_routine routine):
  arg_(arg), routine_(routine)
{
}


void GenerateRandom(vector<int>& vi, int size, int maxV)
{
  set<int> s;
  while(s.size() != size)
  {
    s.insert(rand() % maxV);
  }
  vector<int> temp(s.begin(), s.end());
  vi.swap(temp);
}

int customize_rand( std::size_t k)
{
  unsigned int x = 0;
  return rand_r() % k;
}
void random(void* data)
{
  vector<int>& vi = *static_cast<vector<int>* >(data);
  int rand_count = 10000, v = 0;
  timeval start, end;
  gettimeofday(&start, NULL);
  for(int i = 0; i != rand_count; i++)
  {
    std::random_shuffle(vi.begin(), vi.end(), customize_rand);
    v += vi[0];
  }
  gettimeofday(&end, NULL);
  double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
  cout << "value " << v << std::endl;
  cout << "cost time" << us << std::endl;

}

int main(int argc, char *argv[])
{
  const int thread_num = 3;
  const int data_num = 10000;
  vector<vector<int> > randomData(thread_num, vector<int>());
  for (std::size_t i = 0; i != randomData.size(); i++)
  {
    GenerateRandom(randomData[i], data_num, data_num * 1000);
  }

  ThreadManager tm(thread_num, "testrandom");
  for (std::size_t i = 0; i != randomData.size(); i++)
  {
    tm.AddTask(random, &randomData[i]);
  }

  tm.Start();

  sleep(1000);
  return 0;
}
