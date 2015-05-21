/*
 * author: yuandx
 * create: 2015-05-21
 * email: yuandx@mvad.com
 */
#include <iostream>
#include <time.h>

using std::cout;

int main(int argc, char *argv[])
{
  time_t utc_cur = time(NULL);
  cout << "greenwich time:" << utc_cur << std::endl;
  struct tm* gm_tm = gmtime(&utc_cur);
  cout << "greenwich time: " << asctime(gm_tm) << std::endl;
  cout << "greenwich time: hour: " << gm_tm->tm_hour << std::endl;
  cout << "greenwich time: offset" << gm_tm->tm_gmtoff << std::endl;
  cout << "greenwich timegm: " << timegm(gm_tm) << std::endl;
  struct tm* local_tm = localtime(&utc_cur);
  cout << "local time: " << asctime(local_tm) << std::endl;
  cout << "local time: hour: " << local_tm->tm_hour << std::endl;
  cout << "local time: offset: " << local_tm->tm_gmtoff << std::endl;
  cout << "local mktime: " << mktime(local_tm) << std::endl;
  cout << "after substract offset: " << mktime(local_tm) - local_tm->tm_gmtoff << std::endl;
  cout << "greenwich time calculate: " << timelocal(local_tm) << std::endl;
  return 0;
}
