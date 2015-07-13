#include "../common.h"
#include "../monitor.h"
#include <iostream>
#include <string>

using std::cout;
using std::string;
using namespace micro_ad::utils;

struct MonitorCase
{
  string key_;
  Monitor::mon_type type_;
  MonitorCase(const string& key, Monitor::mon_type type):
    key_(key), type_(type)
  {}
};

static const int NCASE = 5;
MonitorCase mc[NCASE] = {
  MonitorCase("latency", Monitor::COUNTER),
  MonitorCase("qps", Monitor::AVERAGE),
  MonitorCase("retriever", Monitor::COUNTER),
  MonitorCase("cache_hit", Monitor::AVERAGE),
  MonitorCase("filter", Monitor::COUNTER)
};


int main(int argc, char *argv[])
{
  Monitor& monitor = Monitor::Instance();
  for (int i = 0;  i < NCASE; i++)
  {
    monitor.CreateClock(mc[i].key_, mc[i].type_);
  }
  monitor.Set("hello", "world");
  monitor.Set("how", "are you");
  monitor.Set("how", "do you do");
  monitor.Set("there", "it is");
  Random rand_;
  while (true)
  {
    usleep(1000);
    monitor.Incr(mc[rand_.Next() % NCASE].key_, rand_.Next() % 10);
  }
  return 0;
}
