//g++ test.cc  -lpthread -ltbb
#include "../common.h"
#include "../thread_manager.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

using namespace micro_ad::utils;
using std::cout;
using std::vector;

void SumVec(void *arg)
{
  Random rand_;
  vector<int>& v = *static_cast<vector<int>*>(arg);
  int sum = 0;
  for (int i = 0; i < 1000; i++)
  {
    for (vector<int>::size_type j = 0; j != v.size(); j++)
    {
      std::swap(v[j], v[rand_.Next() % (v.size() - j) + j]);
    }
    sum += v[0];
  }
  cout << "thread sum:" << sum << std::endl;
}

int main(int argc, char *argv[])
{
  int64_t start = CurrentUSeconds();
  int64_t sum = 0;
  vector<int> tmp;
  for (int i = 0; i < 1000; i++)
  {
    sum += i;
    tmp.push_back(i);
  }
  int64_t val =  CurrentUSeconds() - start;

  cout << "main sum: " << sum << std::endl;
  cout << "time elasp us: " << val << std::endl;

  ThreadManager tm(1, "hello");
  tm.AddTask(SumVec, &tmp);
  tm.Start();
  tm.Stop();
  string a1 = "abceDFg";
  EXPECT_EQ(string("abcedfg"), ToLower(a1));
  const char *a[4] = {"aaa", "bbb", "ccc", "ddd"};
  vector<string> vstring(a, a+sizeof(a)/sizeof(*a));
  EXPECT_EQ(string("aaatbbbtccctddd"), Join(vstring, "t"));
  vector<string> splits = Split("aaatbbbtccctddd", "t");
  for (int i = 0; i != splits.size(); i++)
  {
    std::cout << splits[i] << "\t";
  }
  std::cout << std::endl;
  EXPECT_EQ(splits.size(), 4);
  EXPECT_TRUE(StartWith("", ""));
  EXPECT_TRUE(StartWith("aab", "a"));
  EXPECT_FALSE(StartWith("aa", "aab"));
  sleep(20);
  return 0;
}
