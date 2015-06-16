#include <sys/time.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <stdlib.h>
#include <set>

using std::unordered_map;
using std::vector;
using std::cout;
using std::set;

void GenerateMapRandom(unordered_map<int, int>& m, unordered_map<int, int>::size_type size,
                       int maxV)
{
  while(m.size() != size)
  {
    m[rand() % maxV] = rand();
  }
}

void GenerateRandom(vector<int>& vi,int size, int maxV)
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
  int map_size = 1000000;
  int query_count = 10000000;
  unordered_map<int, int> unmap;
  vector<int> query;
  GenerateMapRandom(unmap, map_size, map_size*1000);
  GenerateRandom(query, query_count, query_count*1000);
  int count = 0;

  timeval start, end;
  gettimeofday(&start, NULL);

  for (vector<int>::iterator it = query.begin(); it != query.end(); ++it)
  {
    if (unmap.find(*it) != unmap.end())
    {
      count++;
    }
  }
  cout << "query map match: " << count << std::endl;
  gettimeofday(&end, NULL);
  double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
  cout << "cost time" << us << std::endl;


  count = 0;
  gettimeofday(&start, NULL);

  for (int i = 0; i != query_count; i++)
  {
    if (rand() % 2 == 1)
    {
      count++;
    }
  }
  cout << "rand count: " << count << std::endl;
  gettimeofday(&end, NULL);
  us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
  cout << "cost time" << us << std::endl;

  return 0;
}
