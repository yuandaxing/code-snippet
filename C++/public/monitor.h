/*
 * author: yuandx
 * create: 2015-06-13
 * email: yuandx@mvad.com
 */
#ifndef MICROAD_HTTPSERVER_MONITOR_H_
#define MICROAD_HTTPSERVER_MONITOR_H_
#include <string>
#include <list>
#include <tbb/concurrent_hash_map.h>
#include <stdint.h>
#include <stdexcept>
namespace micro_ad
{
namespace utils
{
using std::list;
using std::string;
using tbb::concurrent_hash_map;

class MonitorType
{
public:
  virtual void Incr(int64_t v) = 0;
  virtual MonitorType* Create() const = 0;
  virtual void Add(MonitorType* mt) = 0;
  virtual int64_t Value() const = 0;
  virtual ~MonitorType(){}
};

class AverageMonitor: public MonitorType
{
public:
  AverageMonitor();
  void Incr(int64_t v);
  MonitorType* Create() const;
  void Add(MonitorType* mt);
  int64_t Value() const;
private:
  int64_t count_, value_;
};

class CounterMonitor: public MonitorType
{
public:
  CounterMonitor();
  virtual void Incr(int64_t v);
  virtual MonitorType* Create() const;
  virtual void Add(MonitorType* mt);
  virtual int64_t Value() const;
private:
  int64_t count_;
};

/**
 * the basic algorithm is to mimic the clock
 * push very instant one to the last_xx_xx,
 * at the same time add to instant one to the cur_hand_
 * push front, pop back,
 * for statistics, read from the front
 * update every 1 second
 */
struct ClockMimic
{
public:
  ClockMimic(MonitorType* mt);
  ClockMimic();
  ClockMimic(const ClockMimic& other);
  void operator=(const ClockMimic&);

  void Update(struct tm& time_struct);
  void Incr(int64_t val);
  ~ClockMimic();
  string ToString();

private:
  static void Resize(list<MonitorType*> list_monitor, std::size_t);
  bool IsRewindSecond(struct tm& time_struct);
  bool IsRewindMin(struct tm& time_struct);
  bool IsRewindHour(struct tm& time_struct);
  bool IsRewindDay(struct tm& time_struct);

public:
  enum {SECOND = 0, MINUTE = 1, HOUR = 2, DAY = 3, SIZE = 4 };
  enum {SECOND_NUM = 60, MINUTE_NUM = 60, HOUR_NUM = 24, DAY_NUM = 10 };
  int counter_[SIZE];
  MonitorType* cur_hand_[SIZE], *instant_one_second_;
  list<MonitorType*> last_60_seconds_, last_60_minutes_, last_24_hours_, last_10_days_;
};

class Monitor
{
public:
  enum mon_type {AVERAGE = 0, COUNTER = 1};
  bool Incr(const string& key, int64_t val);
  bool CreateClock(const string& key, mon_type mt);
  bool Set(const string& key, const string& val);
  void Detail(const string& key, string& content);
  void Summary(string& content);
  static Monitor& Instance();

private:
  void Update();
  static void* UpdateWrapper(void* monitor_pointer);
  Monitor();
  //no copy
  Monitor(const Monitor&);
  void operator=(const Monitor&);

private:
  static Monitor* instance_;
  static pthread_mutex_t mutex_;
  bool is_running_;
  concurrent_hash_map<string, ClockMimic> monitor_data_;
  concurrent_hash_map<string, string> key_val_;
  typedef concurrent_hash_map<string, ClockMimic>::accessor clock_iterator;
  typedef concurrent_hash_map<string, string>::accessor kv_iterator;
};
}
}

#endif // MICROAD_HTTPSERVER_MONITOR_H_
