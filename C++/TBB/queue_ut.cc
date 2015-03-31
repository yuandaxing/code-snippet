#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "queue_hashmap.h"

using namespace concurrent_common;
TEST(integrity, queue)
{
  Queue<int> q;
  q.push(1);
  q.push(2);
  int v;
  EXPECT_TRUE(q.try_pop(v));
  EXPECT_EQ(v, 1);
  EXPECT_TRUE(q.try_pop(v));
  EXPECT_EQ(v, 2);
  EXPECT_FALSE(q.try_pop(v));
}

int main(int argc,char** argv) {
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
  return 0;
}
