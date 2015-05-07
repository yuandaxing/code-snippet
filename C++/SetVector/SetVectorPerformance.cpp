#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sys/time.h>
#include <vector>
#include <algorithm>

using std::vector;
using std::cout;
using std::cin;
using std::set;
typedef vector<int>::size_type VISIZETYPE;
typedef vector<int>::iterator  VIITERATOR;
typedef set<int>::size_type SISIZETYPE;

bool BinarySearch(vector<int>& large, int v, int i, int& result)
{
  int left = i, right = static_cast<int>(large.size() - 1); //(left, right]
  while (left < right - 1)
  {
    VISIZETYPE mid = left +  ((right - left) >> 1);
    if (large[mid] < v)
    {
      left = mid;
    } else
    {
      right = mid;
    }
  }
  result = right;
  return right >= 0 && large[right] == v;
}

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

int CombinationSwap(vector<vector<int> >& v1, vector<vector<int> > &v2, int num)
{
  int i = 0;
  while(num != 0)
  {
    if (num&1)
    {
      v1[i].swap(v2[i]);
    }
    i++;
    num /= 2;
  }
}


int main(int argc, char *argv[])
{
  int maxV = 0, size = 0;
  cout << "please input size(int), maxV";
  cin >> size >> maxV;

  vector<int> vi;
  GenerateRandom(vi, size, maxV);
  cout << "finish generate set and vector numbers" << std::endl;
  set<int> si(vi.begin(), vi.end());
  int num_of_count = 5000000;
  vector<int> vi_test;

  //  GenerateRandom(vi_test, num_of_count, num_of_count*50);
  GenerateRandom(vi_test, num_of_count, num_of_count*50);

  cout << "finish of generate random numbers" << std::endl;
  timeval start, end;
  int result = 0, find_count = 0; //dummy
  for(int k = 0; k != 2; k++)
  {
    find_count = 0;
    gettimeofday(&start, NULL);
    for (vector<int>::iterator i = vi_test.begin(); i != vi_test.end(); ++i)
    {
      if(BinarySearch(vi, *i, -1, result))
      {
        find_count++;
      }
    }

    gettimeofday(&end, NULL);
    double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "binary search findCount " << find_count << std::endl;
    cout << "binary search  cost time" << us << std::endl;
    cout << "---------------------------------------" << std::endl;
    find_count = 0;
    gettimeofday(&start, NULL);
    for (vector<int>::iterator i = vi_test.begin(); i != vi_test.end(); ++i)
    {
      if(si.find(*i) != si.end())
      {
        find_count++;
      }
    }

    gettimeofday(&end, NULL);
    us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "set search findCount " << find_count << std::endl;
    cout << "set search  cost time" << us << std::endl;
    cout << "---------------------------------------" << std::endl;


    find_count = 0;
    gettimeofday(&start, NULL);
    for (vector<int>::iterator i = vi_test.begin(); i != vi_test.end(); ++i)
    {
      if(std::binary_search(vi.begin(), vi.end(), *i))
      {
        find_count++;
      }
    }
    gettimeofday(&end, NULL);
    us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
    cout << "system binary search findCount " << find_count << std::endl;
    cout << "system binary search  cost time" << us << std::endl;
    cout << "---------------------------------------" << std::endl;

  }
  return 0;
}
