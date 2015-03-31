#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "queue_hashmap.h"
using namespace concurrent_common;

TEST(integrity, hashmap)
{
  HashMap<int, float> cache;
  EXPECT_TRUE(cache.Set(1, 1.0));
  EXPECT_FALSE(cache.Set(1, 2.));
  float v;
  EXPECT_TRUE(cache.Get(1, v));
  EXPECT_EQ(v, 2.);
  EXPECT_FALSE(cache.Get(2, v));
}
