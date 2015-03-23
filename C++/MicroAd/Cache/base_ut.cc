#include "base.h"
#include "gtest/gtest.h"

TEST(CacheBase, test1)
{
  using MicroAd::Utils::CacheBase;
  CacheBase<int, int> iiCache;
  iiCache.Set(1, 1);
  iiCache.Set(2, 1);
  ASSERT_EQ(iiCache.Size(), 2);
  int i;
  ASSERT_TRUE(iiCache.Get(1, i));
  ASSERT_EQ(i, 1);
  ASSERT_TRUE(iiCache.Get(2, i));
  ASSERT_EQ(i, 1);
  iiCache.Erase(1);
  ASSERT_EQ(iiCache.Size(), 1);
  iiCache.Set(2, 2);
  iiCache.Get(2, i);
  ASSERT_EQ(i, 2);
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int returnValue;
    //Do whatever setup here you will need for your tests here
    //
    //
    returnValue =  RUN_ALL_TESTS();
    //Do Your teardown here if required
    //
    //

    return returnValue;
}
