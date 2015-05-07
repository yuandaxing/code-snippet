#include "gtest/gtest.h"
#include <iostream>
#include <vector>

TEST(blahTest, blah1) {
    EXPECT_EQ(1, 1);
    EXPECT_EQ(1, 2) << "1 and 2 are of unequal\n";
    int a[] = {1, 2, 3, 5};
    std::vector<int> v1(a, a+sizeof(a)/sizeof(*a)), v2(a, a+sizeof(a)/sizeof(*a));
    std::cout << v1.size() << std::endl;
    EXPECT_EQ(v1.size(), v2.size()) << "v1 and v2 have different size";
}
TEST(test1, blah1)
{
  ASSERT_EQ(1, 2);
  int a[] = {2,3,3,4};
  for(std::size_t i = 0; i != sizeof(a)/sizeof(*a); i++)
  {
    ASSERT_EQ(a[i], a[i]);
  }
  for(int i = 0; i < 10; i++)
  {
    ASSERT_LT(i, 4);
  }
  for(int i = 0; i < 10; i++)
  {
    ASSERT_NE(i, 0);
  }
  ASSERT_TRUE(1);
  ASSERT_FALSE(1);

}


TEST(a, b)
{
  ASSERT_STREQ("aa", "bb");
  ASSERT_STREQ("aa", "aa");

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
