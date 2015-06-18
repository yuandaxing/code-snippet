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
  virtual void Incr(int64_t v) = 0;
  virtual MonitorType* Create() = 0;
  virtual void Add(MonitorType* mt) = 0;
  virtual ~MonitorType(){}
};

class AverageMonitor: public MonitorType
{
public:
  AverageMonitor();
  void Incr(int64_t v);
  MonitorType* Create();
  void Add(MonitorType* mt);
private:
  int64_t count_, value_;
};

class QpsMonitor: public MonitorType
{
public:
  QpsMonitor();
  virtual void Incr(int64_t v) = 0;
  virtual MonitorType* Create() = 0;
  virtual void Add(MonitorType* mt) = 0;
private:
  int64_t count_;
};
/**
 * the basic algorithm is to check the current time
 */
struct Calendar
{
  Calendar(MonitorType* mt);
  static void Resize(list<MonitorType*> list_monitor, std::size_t);

  enum {SECOND = 0, MINUTE = 1, HOUR = 2, DAY = 3, SIZE = 4};
  MonitorType* cur_[SIZE];
  list<MonitorType*> last_60_seconds_, last_60_minutes_, last_24_hours_, last_10_days_;
};

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
