#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sys/time.h>
#include <vector>
#include <map>

using std::vector;
using std::cout;
using std::cin;
using std::set;
using std::map;

typedef vector<int>::size_type VISIZETYPE;
typedef vector<int>::iterator  VIITERATOR;
typedef set<int>::size_type SISIZETYPE;

void GenerateRandom(vector<int>& vi, SISIZETYPE size, int maxV)
{
  set<int> s;
  while(s.size() != size)
  {
    s.insert(rand() % maxV);
  }
  vector<int> temp(s.begin(), s.end());
  vi.swap(temp);
}

void GenerateMapRandom(map<int, int>& m, map<int, int>::size_type size,
                       int maxV)
{
  while(m.size() != size)
  {
    m[rand() % maxV] = rand();
  }
}


int main(int argc, char *argv[])
{
  int base_size, maxV, vec_count = 20;
  int query_count;
  vector<vector<int> > vvi1(vec_count, vector<int>()), vvi2(vvi1);
  cout << "please input base_size(int), maxV(int), query_count(int)";
  cin >> base_size >> maxV >> query_count;
  map<int, int> test_map;
  vector<int> query;
  GenerateRandom(query, query_count, maxV);
  GenerateMapRandom(test_map, base_size, maxV);


  {
    timeval start, end;
    gettimeofday(&start, NULL);
    int count = 0;
    for(int i = 0; i != 1000; i++)
    {
      for(vector<int>::iterator it = query.begin(); it != query.end();
          ++it)
      {
        if(test_map.find(*it) != test_map.end())
        {
          count++;
        }
      }
    }
    cout << "query map match: " << count << std::endl;
    gettimeofday(&end, NULL);
    double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "cost time" << us << std::endl;
  }

  return 0;
}
