#include "file_lock.h"
#include "gtest/gtest.h"
using MicroAd::Utils::FileLocker;

TEST(FileLocker, basic)
{
  FileLocker fc("./filelocker");
  EXPECT_TRUE(fc.Open());
  EXPECT_EQ(fc.TryLock(), 0);
  EXPECT_EQ(fc.TryLock(), 0);
  const std::string content = "hello world";
  std::string result;
  EXPECT_TRUE(fc.TruncWrite(content));
  EXPECT_TRUE(fc.Read(result));
  getchar();
  std::cerr << "result :" << result << std::endl;
}
int main (int argc, char** argv)
{
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
