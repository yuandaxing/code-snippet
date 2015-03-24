#include "gtest/gtest.h"
#include <serv.h>
#include <request_mgr.h>
#include <stdio.h>
#include <iostream>
#include <param_repository.h>
using namespace MicroAd::Utils;

DEFINE_BOOL(enable_whitelist, "false", true, "hello world, this is a test for enable whitelist");
DECLARE_BOOL(enable_whitelist);
DEFINE_INT32(int32_whitelist, "11", false, "hello, world, testing for int32_whitelist");
DECLARE_INT32(int32_whitelist);
DEFINE_INT64(int64_whitelist, "111111111111", true, "testing for int64 type");
DECLARE_INT64(int64_whitelist);
DEFINE_STRING(string_whitelist, "/path/t/a/b", false, "testing for string whitelist");
DECLARE_STRING(string_whitelist);

using namespace MicroAd::Utils;
TEST(ParamRepository, Test)
{
  BOOL_SET(enable_whitelist, "true", ValSrc::COMMAND);
  ASSERT_TRUE(enable_whitelist);
  ASSERT_EQ(int32_whitelist, 11);
  ASSERT_STREQ(string_whitelist.c_str(), "/path/t/a/b");
}
int main(int argc, char *argv[])
{

  ::testing::InitGoogleTest(&argc, argv);
  int returnValue;
  //Do whatever setup here you will need for your tests here
  //
  //
  returnValue =  RUN_ALL_TESTS();
  std::shared_ptr<IHttpRequestProcessor> hrp(new RequestMgr());
  Serv* serv = Serv::Instance();
  if(0 != serv->Start(hrp, 8000))
  {
    std::cout << "could not start server" << std::endl;
  }
  getchar();
  serv->Stop();
  return returnValue;
}
