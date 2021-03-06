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
  int left = i, right = static_cast<int>(large.size()); //[left, right), lower bound algorithm
  while (left < right - 1)
  {
    VISIZETYPE mid = left +  ((right - left) >> 1);
    if (large[mid] <= v)
    {
      left = mid;
    } else
    {
      right = mid;
    }
  }
  result = left;
  return left >= 0 && large[left] == v;
}

void BinaryIntersection(vector<int>& large, vector<int>& small, vector<int>& result)
{
  int pos = -1, occur = 0;
  int i = -1, j = 0;
  int size1 = large.size(), size2 = small.size();
  while (i < size1 && j < size2)
  {
    if (BinarySearch(large, small[j], i, occur))
    {
      result.push_back(small[j]);
    }
    i = occur + 1;

    if ( i < size1 && BinarySearch(small, large[i], j, occur))
    {
      result.push_back(large[i]);
    }
    j = occur + 1;
  }
}

void FastIntersect(vector<int>& v1, vector<int>& v2,
                   vector<int>& result, double factor = 1)
{
  result.reserve(static_cast<int>(v1.size()));
  if ((v1.size() << 4) < v2.size())
  {
    BinaryIntersection(v1, v2, result);
  } else
  {
    NormalIntersect(v1, v2, result);
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
void SimpleUnionIntersection(vector<vector<int> >& Bs, vector<int>& A1, vector<int>& result)
{
  vector<int> temp1, temp2;
  for(vector<vector<int> >::iterator it = Bs.begin(); it != Bs.end(); ++it)
  {
    Merge2(*it, temp1, temp2);
    temp1.swap(temp2);
    temp2.clear();
  }
  FastIntersect(A1, temp1, result);
}

void SmartUnionIntersection(vector<vector<int> >& Bs, vector<int>& A1, vector<int>& result)
{
  vector<vector<int> > rs(Bs.size(), vector<int>() );
  for(vector<vector<int> >::size_type i = 0; i != Bs.size(); ++i)
  {
    if (A1.size() > Bs[i].size())
    {
      FastIntersect(A1, Bs[i], rs[i]);
    } else
    {
      FastIntersect(Bs[i], A1, rs[i]);
    }
  }

  vector<int> temp1, temp2;
  for(vector<vector<int> >::iterator it = rs.begin(); it != rs.end(); ++it)
  {
    Merge2(*it, temp1, temp2);
    temp1.swap(temp2);
    temp2.clear();
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



int main(int argc, char *argv[])
{
  int base_size, maxV, vec_count = 20;
  int base_vector_size = 10;
  double incr_fact;
  double  us1 = 0, us2 = 0;
  vector<vector<int> > vvi1(vec_count, vector<int>()), vvi2(vvi1);
  vector<int> A1;
  cout << "please input base_size(int), maxV(int), incr_fact(double)";
  cin >> base_size >> maxV >> incr_fact;

  int num_of_count = base_size;
  timeval start, end;
  gettimeofday(&start, NULL);
  GenerateRandom(A1, base_vector_size, maxV);
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
    max_swap /= 1000;

    {
      size = 0;
      cout << "----------------------------------------" << std::endl
           << std::endl;
      int count = 0;
      gettimeofday(&start, NULL);
      for(int i = 0; i != max_swap; i++)
      {
        CombinationSwap(vvi1, vvi2, i);
        count += 1;
        CombinationSwap(vvi1, vvi2, i);
        result.clear();
      }
      gettimeofday(&end, NULL);
      us1 = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "count " << count << std::endl;
      cout << "count cost time" << us1 << "us" << std::endl;
      cout << "----------------------------------------" << std::endl;
      cout << std::endl;
    }

    {
      size = 0;
      cout << "----------------------------------------" << std::endl
           << std::endl;
      gettimeofday(&start, NULL);
      for(int i = 0; i != max_swap; i++)
      {
        CombinationSwap(vvi1, vvi2, i);
        SimpleUnionIntersection(vvi1, A1, result);
        size += result.size();
        CombinationSwap(vvi1, vvi2, i);
        result.clear();
      }
      gettimeofday(&end, NULL);
      us1 = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "SimpleUnionIntersection size: " << size << std::endl;
      cout << "SimpleUnionIntersection cost time" << us1 << "us" << std::endl;
      cout << "----------------------------------------" << std::endl;
      cout << std::endl;
    }

    {
      size = 0;
      cout << "----------------------------------------" << std::endl << std::endl;
      gettimeofday(&start, NULL);
      for(int i = 0; i != max_swap ; i++)
      {
        CombinationSwap(vvi1, vvi2, i);
        SmartUnionIntersection(vvi1, A1, result);
        size += result.size();
        CombinationSwap(vvi1, vvi2, i);
        result.clear();
      }

      gettimeofday(&end, NULL);
      us2 = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
      cout << "SmartUnionIntersection size: " << size << std::endl;
      cout << "SmartUnionIntersection cost time" << us2 << "us" << std::endl;
      cout << "----------------------------------------" << std::endl;
    }
    cout << "ratios is " << (us1 / us2) << std::endl;
  }

  return 0;
}
