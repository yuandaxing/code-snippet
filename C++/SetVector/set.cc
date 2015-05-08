#include <sys/time.h>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <unordered_set>
#include <vector>

using std::cout;
using std::set;
using std::cin;
using std::unordered_set;
using std::vector;

int find_unordered_set(unordered_set<int64_t>& s, vector<int64_t>& result)
{
  int count = 0;
  for(vector<int64_t>::iterator it = result.begin(); it != result.end(); ++it)
  {
    if(s.find(*it) != s.end())
    {
      ++count;
    }
  }
  return count;
}

int find_set(set<int64_t>& s, vector<int64_t>& result)
{
  int count = 0;
  for(vector<int64_t>::iterator it = result.begin(); it != result.end(); ++it)
  {
    if(s.find(*it) != s.end())
    {
      ++count;
    }
  }
  return count;
}

void GenerateRandom(vector<int64_t>& vi, int size, int maxV)
{
  set<int64_t> s;
  // while(s.size() != size)
  // {
  //   s.insert(rand() % maxV);
  // }
for(int i = 0; i != size; i++)
 {
s.insert(i);
}
  vector<int64_t> temp(s.begin(), s.end());
  vi.swap(temp);
}

int main(int argc, char *argv[])
{

  int set_size, find_count;
  cout << "please input set_size(int), find_count(int):";
  cin >> set_size >> find_count;
  int max_v = find_count * 100; 
  vector<int64_t> temp, result;
  
  GenerateRandom(temp, set_size, max_v);
  GenerateRandom(result, find_count, max_v);
  set<int64_t> set(temp.begin(), temp.end());
  unordered_set<int64_t> hashset(temp.begin(), temp.end());
hashset.max_load_factor(0.1);
for(int i = 0; i < 2; i++)
  {
      timeval start, end;
      gettimeofday(&start, NULL);
      cout << "hash result:" << find_unordered_set(hashset, result) << std::endl;
      gettimeofday(&end, NULL);
      double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
cout << "hash take time " << us << "load_factor" << hashset.load_factor() <<  std::endl << std::endl;

      gettimeofday(&start, NULL);
      cout << "set result:" << find_set(set, result) << std::endl;
      gettimeofday(&end, NULL);
      us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "set take time " << us << std::endl << std::endl;
  }

  return 0;
}
