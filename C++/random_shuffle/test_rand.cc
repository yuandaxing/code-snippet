#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sys/time.h>
#include <vector>
#include <map>
#include <stdlib.h>
using std::vector;
using std::cout;
using std::cin;
using std::set;
using std::map;

void GenerateMapRandom(map<int, int>& m, map<int, int>::size_type size,
                       int maxV)
{
  while(m.size() != size)
  {
    m[rand() % maxV] = rand();
  }
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

int main(int argc, char *argv[])
{
  int testCase = 10000;
  int rand_count = 10000;
  vector<int> vi;
  int v = 0;
  GenerateRandom(vi, testCase, testCase*1000);
  {
    timeval start, end;
    gettimeofday(&start, NULL);
    for(int i = 0; i != rand_count; i++)
    {
      std::random_shuffle(vi.begin(), vi.end());
      v += vi[0];
    }
    gettimeofday(&end, NULL);
    double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "value " << v << std::endl;
    cout << "cost time" << us << std::endl;
  }

  {
    timeval start, end;
    gettimeofday(&start, NULL);
    size_t cnt = 0;
    for(int i = 0; i != rand_count; i++)
    {
      cnt += std::rand();
    }
    gettimeofday(&end, NULL);
    double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "value " << cnt << std::endl;
    cout << "cost time" << us << std::endl;
  }


  {
    timeval start, end;
    gettimeofday(&start, NULL);
    size_t cnt = 0;
    for(int i = 0; i != rand_count; i++)
    {
      cnt += rand();
    }
    gettimeofday(&end, NULL);
    double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "value " << cnt << std::endl;
    cout << "cost time" << us << std::endl;
  }


  return 0;
}
