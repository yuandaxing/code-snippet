#include <pthread.h>
#include <iostream>

using std::cout;
void* function(void *arg)
{
  cout << "hello world" << std::endl;
}


int main(int argc, char *argv[])
{
  int threads = 20;
  for (int i = 0; i != threads; ++i)
  {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, function, NULL);
    cout << "create thread " << thread_id << std::endl;
  }

  return 0;
}
