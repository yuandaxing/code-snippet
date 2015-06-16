/*
 * author: yuandx
 * create: 2015-06-13
 * email: yuandx@mvad.com
 */
#ifndef MICROAD_HTTPSERVER_MONITOR_H_
#define MICROAD_HTTPSERVER_MONITOR_H_

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
};

class AverageMonitor: public MonitorType
{
public:
  void Incr(int64_t v);
  MonitorType* Create();
private:
  int64_t count_, value_;
};

class QpsMonitor: public MonitorType
{

};
/**
 * the basic algorithm is to check the current time
 */
class Calendar
{
public:
  void Update(time_t t);
private:
  void Resize(list<MonitorType*> list_monitor, std::size_t);

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

private:
  using std::string;
  using tbb:concurrent_hash_map;

  concurrent_hash_map<string, MonitorType*> data_;
};
}
}





#endif // MICROAD_HTTPSERVER_MONITOR_H_
