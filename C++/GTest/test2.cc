#include <cstdio>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <errno.h>
#include <algorithm>
#include <semaphore.h>
#include <functional>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

using std::string;

time_t StrToSeconds(const string& sTimeStr)
{
  struct tm temp;
  memset(&temp, 0, sizeof(tm));
  strptime(sTimeStr.c_str(), "%Y-%m-%d %H:%M:%S", &temp);
  return mktime(&temp);
}
int main(int argc, char *argv[])
{
  std::string str1 = "2037-12-31 23:59:00", str2 = "2036-12-31 23:59:00", str3 = "2199-12-12 23:59:00";
  std::cout << StrToSeconds(str1) << std::endl;
  std::cout << "str1 > str2:" << (StrToSeconds(str1) > StrToSeconds(str2)) << std::endl;
  std::cout << "str3 > str2:" << (StrToSeconds(str3) > StrToSeconds(str2)) << std::endl;
  std::cout << StrToSeconds(str3) << std::endl;
  return 0;
}
