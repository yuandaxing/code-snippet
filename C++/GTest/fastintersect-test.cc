#include <iostream>
#include <vector>

using std::vector;
typedef vector<int> PostList;
void NormalIntersect(const PostList& list1, const PostList& list2, PostList& result)
{
  for (PostList::const_iterator it1 = list1.begin(), it2 = list2.begin();
       it1 != list1.end() && it2 != list2.end();)
  {
    PostList::value_type e1 = *it1, e2 = *it2;
    if (e1 < e2)
    {
      ++it1;
    }
    else if (e1 > e2)
    {
      ++it2;
    }
    else
    {
      result.push_back(e1);
      ++it1;
      ++it2;
    }
  }
}

bool BinarySearch(const PostList& lists, const int v, int i, int& result)
{
  int left = i, right = static_cast<int>(lists.size()); //[left, right), lower bound algorithm
  while (left < right - 1)
  {
    int mid = left +  ((right - left) >> 1);
    if (lists[mid] <= v)
    {
      left = mid;
    }
    else
    {
      right = mid;
    }
  }
  result = left;
  return left >= 0 && left < static_cast<int>(lists.size()) && lists[left] == v;
}

void BinaryIntersect(const PostList& small, const PostList& large, PostList& result)
{
  int occur = 0;
  int i = 0, j = -1;
  int size1 = static_cast<int>(small.size()), size2 = static_cast<int>(large.size());

  while (i < size1 && j < size2)
  {
    if (BinarySearch(large, small[i], j, occur))
    {
      result.push_back(small[i]);
    }
    j = occur + 1;

    if (j < size2 && BinarySearch(small, large[j], i, occur))
    {
      result.push_back(large[j]);
    }
    i = occur + 1;
  }
}

void FastIntersectAux(const PostList& list1, const PostList& list2, PostList& result)
{
  result.reserve(list1.size());
  if ((list1.size() << 2) <= list2.size())
  {
    BinaryIntersect(list1, list2, result);
  }
  else
  {
    NormalIntersect(list1, list2, result);
  }
}

void FastIntersect(const PostList& list1, const PostList& list2, PostList& result)
{
  if (list1.size() < list2.size())
  {
    FastIntersectAux(list1, list2, result);
  }
  else
  {
    FastIntersectAux(list2, list1, result);
  }
}

class TestCase
{
public:
  PostList list1_, list2_, result_;
  TestCase(int* beg1, int* end1, int* beg2, int* end2, int* beg_r1, int* end_r1)
  {
    list1_.insert(list1_.end(), beg1, end1);
    list2_.insert(list2_.end(), beg2, end2);
    result_.insert(result_.end(), beg_r1, end_r1);
  }
  bool Run()
  {
    PostList temp;
    FastIntersect(list1_, list2_, temp);
    if (temp.size() != result_.size())
    {
      return false;
    }
    for (std::size_t i = 0; i != result_.size(); i++)
    {
      if (temp[i] != result_[i])
      {
        return false;
      }
    }
    return true;
  }
};

using std::cout;
int main(int argc, char *argv[])
{
  int a1[] = {1, 2, 3}, b1[] = {}, c1[] = {};
  cout << "testCase(a1, a1+3, b1, b1, c1, c1): " << TestCase(a1, a1+3, b1, b1, c1, c1).Run() << std::endl;
  cout << "testCase(a1, a1+3, b1, b1, c1, c1): " << TestCase(b1, b1, a1, a1+3, c1, c1).Run() << std::endl;

  int a2[] = {1, 2, 3, 4, 5}, b2[] = {1}, c2[]={1};
  cout << "TestCase(a2, a2+5, b2, b2+1, c2, c2+1): " << TestCase(a2, a2+5, b2, b2+1, c2, c2+1).Run()
       << std::endl;
  cout << "TestCase(b2, b2+1, a2, a2+5, c2, c2+1): " << TestCase(b2, b2+1, a2, a2+5, c2, c2+1).Run()
       << std::endl;

  int a3[] = {1, 2, 3, 4, 5}, b3[] = {5}, c3[]={5};
  cout << "TestCase(a3, a3+5, b3, b3+1, c3, c3+1): " << TestCase(a3, a3+5, b3, b3+1, c3, c3+1).Run()
       << std::endl;
  cout << "TestCase(b3, b3+1, a3, a3+5, c3, c3+1): " << TestCase(b3, b3+1, a3, a3+5, c3, c3+1).Run()
       << std::endl;


  int a4[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}, b4[] = {0, 5}, c4[]={5};
  cout << "TestCase(a4, a4+9, b4, b4+2, c4, c4+1): " << TestCase(a4, a4+9, b4, b4+2, c4, c4+1).Run()
       << std::endl;
  cout << "TestCase(b4, b4+2, a4, a4+9, c4, c4+1): " << TestCase(b4, b4+2, a4, a4+9, c4, c4+1).Run()
       << std::endl;

  int a5[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}, b5[] = {5,10}, c5[]={5};
  cout << "TestCase(a5, a5+9, b5, b5+2, c5, c5+1): " << TestCase(a5, a5+9, b5, b5+2, c5, c5+1).Run()
       << std::endl;
  cout << "TestCase(b5, b5+2, a5, a5+9, c5, c5+1): " << TestCase(b5, b5+2, a5, a5+9, c5, c5+1).Run()
       << std::endl;


  int a6[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}, b6[] = {1, 5}, c6[]={1, 5};
  cout << "TestCase(a6, a6+9, b6, b6+2, c6, c6+2): " << TestCase(a6, a6+9, b6, b6+2, c6, c6+2).Run()
       << std::endl;
  cout << "TestCase(b6, b6+2, a6, a6+9, c6, c6+2): " << TestCase(b6, b6+2, a6, a6+9, c6, c6+2).Run()
       << std::endl;

  int a7[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}, b7[] = {2, 10}, c7[]={2};
  cout << "TestCase(a7, a7+9, b7, b7+2, c7, c7+1): " << TestCase(a7, a7+9, b7, b7+2, c7, c7+1).Run()
       << std::endl;
  cout << "TestCase(b7, b7+2, a7, a7+9, c7, c7+1): " << TestCase(b7, b7+2, a7, a7+9, c7, c7+1).Run()
       << std::endl;

  int a8[] = {1, 2, 3, 4, 5, 6, 7, 8, 9}, b8[] = {2, 3, 10}, c8[]={2, 3};
  cout << "TestCase(a8, a8+9, b8, b8+3, c8, c8+2): " << TestCase(a8, a8+9, b8, b8+3, c8, c8+2).Run()
       << std::endl;
  cout << "TestCase(b8, b8+3, a8, a8+9, c8, c8+2): " << TestCase(b8, b8+3, a8, a8+9, c8, c8+2).Run()
       << std::endl;

  return 0;
}
