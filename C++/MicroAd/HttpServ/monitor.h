/*
 * author: yuandx
 * create: 2015-06-13
 * email: yuandx@mvad.com
 */
#ifndef MICROAD_HTTPSERVER_MONITOR_H_
#define MICROAD_HTTPSERVER_MONITOR_H_
#include <string>
#include <list>
#include <tbb/concurrent_hash_map>

namespace MicroAd
{
namespace Utils
{
class MonitorType
{
public:
  void Incr(int64_t v) = 0;
  void InitOrIncr(int64_t v) = 0;
};

class AverageMonitor: public MonitorType
{

};

class QpsMonitor: public MonitorType
{

};

class Calendar
{
public:

private:
  enum {SECONDS=0, }

}

class Monitor
{
public:
  void Incr(const string& key, int64_t val);
  void InitOrIncr(const string& key, int64_t);
  void Detail(const string& key, string& content);
  void Summary(string& content);
  void Set(const string& key, const string& val);

private:
  using std::string;
  using tbb:concurrent_hash_map;

  concurrent_hash_map<string, MonitorType*> monitor_data_;
  concurrent_hash_map<string, string> key_val_;
};
}
}

#endif // MICROAD_HTTPSERVER_MONITOR_H_
