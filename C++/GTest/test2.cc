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

const int BUFF_SIZE = 1024;



using std::string;

time_t StrToSeconds(const string& sTimeStr)
{
  struct tm temp;
  memset(&temp, 0, sizeof(tm));
  strptime(sTimeStr.c_str(), "%Y-%m-%d %H:%M:%S", &temp);
  return mktime(&temp);
}


using std::vector;
using std::string;

void GenerateIndexCacheKey(const uint64_t mask, const vector<int64_t>& labels, string& key)
{
  char temp[BUFF_SIZE] = {0};
  int pos = 0;

  pos += snprintf(temp + pos, BUFF_SIZE - pos, "%lxl", mask);
  for (vector<int64_t>::const_iterator iter = labels.begin(); iter != labels.end(); ++iter)
  {
    pos += snprintf(temp + pos, BUFF_SIZE - pos, "%lx_", (0xFFFFFFFFFFFFFF & *iter));  // Hide the high 8 bit
    if (BUFF_SIZE - pos < 16)
    {
      key.append(temp);
      memset(temp, 0, BUFF_SIZE);
      pos = 0;
    }
  }
  key.append(temp);
}


int main(int argc, char *argv[])
{
  std::string str1 = "2037-12-31 23:59:00", str2 = "2036-12-31 23:59:00", str3 = "2199-12-12 23:59:00";
  std::cout << StrToSeconds(str1) << std::endl;
  std::cout << "str1 > str2:" << (StrToSeconds(str1) > StrToSeconds(str2)) << std::endl;
  std::cout << "str3 > str2:" << (StrToSeconds(str3) > StrToSeconds(str2)) << std::endl;
  std::cout << StrToSeconds(str3) << std::endl;
  uint64_t mask = 0xff11;
  uint64_t data[] = {0x11, 0x33, 0xff, 0xaa};
  vector<int64_t> label(data, data+4);
  string result;
  std::cout << "cache key result ---------------" << std::endl;
  GenerateIndexCacheKey(mask, label, result);
  std::cout << result << std::endl;
  vector<int64_t> label2(512, 0x11);
  result.clear();
  GenerateIndexCacheKey(mask, label2, result);
  std::cout << result << std::endl;
  std::cout << result.size() << std::endl;
  return 0;
}
