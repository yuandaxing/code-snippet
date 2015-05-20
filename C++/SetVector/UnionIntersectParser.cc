#include <iostream>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>
#include <sys/time.h>
#include <vector>
#include <list>
#include <utility>

template<typename T>
void NoCopyAdd(list<T>& dest, T& element)
{
  dest.push_back(T());
  T& last = dest.back();
  last.swap(element);
}
template<typename T>
void NoCopyAdd(vector<T>& dest, T& element)
{
  dest.push_back(T());
  T& last = dest.back();
  last.swap(element);
}

class UionIntersecParser
{
public:
  typedef vector<vector<int>* > DimUion;
  UionIntersecParser(){}
  //this function may change the du to avoid copy memory
  void AddUion(DimUion& du)
  {
    int size = 0;
    for(DimUion::iterator it = du.begin(); it != du.end(); ++it)
    {
      size += (**it).size();
    }
    size = du.size() <= 1 ? du.size() : size;

    pair<int, DimUion()> element = make_pair(size, DimUion());
    element.second.swap(du);
    NoCopyAdd(dim_uion_intersect_, element);
  }
private:
  int compare(pair<int, DimUion>& lhs, pair<int, DimUion>& rhs)
  {
    int ret = lhs.first - rhs.first;
    if (0 == ret)
    {
      ret = (lhs.first == 1) ? (lhs.second[0]->size() - rhs.second[0]->size()) : ret;
    }
    return ret;
  }
  void Sort()
  {
    std::sort();

  }
  list<vector<int> > temp_result_;
  list<pair<int, DimUion> > dim_uion_intersect_;
};
