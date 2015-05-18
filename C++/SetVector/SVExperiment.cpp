#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sys/time.h>
#include <vector>

using std::vector;
using std::cout;
using std::cin;
using std::set;
typedef vector<int>::size_type VISIZETYPE;
typedef vector<int>::iterator  VIITERATOR;
typedef set<int>::size_type SISIZETYPE;


void NormalIntersect(vector<int>& vec1, vector<int>& vec2, vector<int>& result)
{
  for (VIITERATOR it1 = vec1.begin(), it2 = vec2.begin(); it1 != vec1.end() && it2 != vec2.end();)
  {
    if (*it1 < *it2)
    {
      ++it1;
    } else if (*it1 > *it2)
    {
      ++it2;
    } else
    {
      result.push_back(*it1);
      ++it1;
      ++it2;
    }
  }
}

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
void BinaryIntersection(vector<int>& large, vector<int>& small, vector<int>& result)
{
  int pos = -1, occur = 0;
  for (VIITERATOR it = small.begin(); it != small.end(); ++it)
  {
    if (BinarySearch(large, *it, pos, occur))
    {
      result.push_back(*it);
      pos = occur;
    }
  }
}
void Merge2(vector<int>& v1, vector<int>& v2, vector<int>& r)
{
  vector<int>::iterator it1 = v1.begin(), it2 = v2.begin();
  while(it1 != v1.end() && it2 != v2.end())
  {
    if(*it1 > *it2)
    {
      r.push_back(*it2);
      ++it2;
    } else if (*it1 < *it2)
    {
      r.push_back(*it1);
      ++it1;
    } else
    {
      r.push_back(*it1);
      ++it1;
      ++it2;
    }
  }
  while(it1 != v1.end())
  {
    r.push_back(*it1);
    ++it1;
  }
  while(it2 != v2.end())
  {
    r.push_back(*it2);
    ++it2;
  }
}
void MergeVec(vector<vector<int> >& v2int, vector<int>& result)
{
  if(v2int.size() == 0) return ;
  vector<int> temp1, temp2;
  temp1.reserve(5000);
  temp2.reserve(5000);
  for(vector<vector<int> >::iterator it = v2int.begin(); it != v2int.end(); ++it)
  {
    Merge2(temp1, *it, temp2);
    temp1.swap(temp2);
    temp2.clear();
  }
  result.swap(temp1);
}

void FastIntersectSolution(vector<vector<int> >& v2int, vector<int>& result, double factor = 0.8)
{
  if (v2int.size() < 1)
  {
    return ;
  }
  vector<int> temp1(v2int[0]), temp2; //temp.swap(v2int[0]) for online implement
  temp2.reserve(static_cast<int>(temp1.size() * factor));

  vector<vector<int> >::iterator it = v2int.begin();

  for (++it; it != v2int.end(); ++it)
  {
    temp2.swap(temp1);
    temp1.clear();

    if ((temp2.size() << 4) < it->size())
    {
      BinaryIntersection(*it, temp2, temp1);
    } else
    {
      NormalIntersect(*it, temp2, temp1);
    }
  }
  result.swap(temp1);
}

void NormalIntersectSolution(vector<vector<int> >& v2int, vector<int>& result, double factor = 0.8)
{
  if (v2int.size() < 1)
  {
    return ;
  }
  vector<int> temp1(v2int[0]), temp2; //temp.swap(v2int[0]) for online implement
  temp2.reserve(static_cast<int>(temp1.size() * factor));

  vector<vector<int> >::iterator it = v2int.begin();
  for (++it; it != v2int.end(); ++it)
  {
    temp2.swap(temp1);
    temp1.clear();
    NormalIntersect(*it, temp2, temp1);
  }
  result.swap(temp1);
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

void PrintVec(vector<int>& vi)
{
  for(VIITERATOR it = vi.begin(); it != vi.end(); ++it)
  {
    cout << *it << "\t";
  }
  cout << std::endl;
}


///////////////////
// main function //
///////////////////
int main(int argc, char *argv[])
{

  int base_size, maxV, vec_count = 20;
  double incr_fact;
  vector<vector<int> > vvi1(vec_count, vector<int>()), vvi2(vvi1);
  cout << "please input base_size(int), maxV(int), incr_fact(double)";
  cin >> base_size >> maxV >> incr_fact;

  int num_of_count = base_size;
  timeval start, end;

  gettimeofday(&start, NULL);
  for(int i = 0; i != vec_count; i++)
  {
    //    num_of_count = static_cast<int>(num_of_count * incr_fact);
    num_of_count = static_cast<int>(base_size + 10 * i * incr_fact);
    GenerateRandom(vvi1[i], num_of_count, maxV);
    GenerateRandom(vvi2[i], num_of_count, maxV);
  }
  gettimeofday(&end, NULL);
  double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
  cout << "generate cost time" << us << std::endl;

  vector<int> result;
  volatile VISIZETYPE size = 0;
  for(int k = 0; k != 2; k++)
  {
    int max_swap = pow(2, vec_count);
    {
      gettimeofday(&start, NULL);
      for(int i = 0; i != max_swap; i++)
      {
        CombinationSwap(vvi1, vvi2, i);
        FastIntersectSolution(vvi1, result);
        size += result.size();
        CombinationSwap(vvi1, vvi2, i);
      }

      gettimeofday(&end, NULL);
      double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "fast Intersection size: " << size << std::endl;
      cout << "fast Intersection cost time" << us << std::endl;

      cout << "----------------------------------------" << std::endl;
    }

    {
      size = 0;
      gettimeofday(&start, NULL);

      for(int i = 0; i != max_swap ; i++)
      {
        CombinationSwap(vvi1, vvi2, i);
        NormalIntersectSolution(vvi1, result);
        size += result.size();
        CombinationSwap(vvi1, vvi2, i);
      }

      gettimeofday(&end, NULL);
      double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "normal Intersection size: " << size << std::endl;
      cout << "normal Intersection cost time" << us << std::endl;
    }

    {
      size = 0;
      int result_size = 0;
      gettimeofday(&start, NULL);
      max_swap /= 1000;
      for(int i = 0; i != max_swap ; i++)
      {
        CombinationSwap(vvi1, vvi2, i);
        MergeVec(vvi1, result);
        size += result.size();
        result_size = result.size();
        result.resize(0);
        CombinationSwap(vvi1, vvi2, i);
      }

      gettimeofday(&end, NULL);
      double us = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "Merge  size: " << size << std::endl;
      cout << "result size: " << result_size << std::endl;
      cout << "Merge cost time" << us << std::endl;
    }
  }
  return 0;
}
