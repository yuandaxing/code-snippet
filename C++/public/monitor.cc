#include "monitor.h"
#include "common.h"

#include <sys/time.h>
#include <stdio.h>

namespace micro_ad
{
namespace utils
{

AverageMonitor::AverageMonitor():
  count_(0), value_(0)
{
}

void AverageMonitor::Incr(int64_t v)
{
  ++count_;
  value_ += v;
}

MonitorType* AverageMonitor::Create() const
{
  return new AverageMonitor();
}

void AverageMonitor::Add(MonitorType *mt)
{
  AverageMonitor* am = dynamic_cast<AverageMonitor*>(mt);
  if (NULL == am)
  {
    throw std::runtime_error("not AverageMonitor type");
  }
  count_ += am->count_;
  value_ += am->value_;
}

int64_t AverageMonitor::Value() const
{
  return value_ / count_;
}
CounterMonitor::CounterMonitor(): count_(0)
{
}

void CounterMonitor::Incr(int64_t v)
{
  count_ += v;
}

MonitorType* CounterMonitor::Create() const
{
  return new CounterMonitor();
}

void CounterMonitor::Add(MonitorType *mt)
{
  CounterMonitor* qm = dynamic_cast<CounterMonitor*>(mt);
  if (NULL == qm)
  {
    throw std::runtime_error("not CounterMonitor type");
  }
  count_ += qm->count_;
}

int64_t CounterMonitor::Value() const
{
  return count_;
}

ClockMimic::ClockMimic(MonitorType* mt)
{
  if (NULL == mt)
  {
    throw std::runtime_error("monitorType NULL");
  }
  for (std::size_t i = 0; i != SIZE; ++i)
  {
    cur_hand_[i] = mt->Create();
  }
  instant_one_second_ = mt;
  struct tm tm;
  time_t cur = time(NULL);
  localtime_r(&cur, &tm);
  counter_[SECOND] = tm.tm_sec;
  counter_[MINUTE] = tm.tm_min;
  counter_[HOUR]   = tm.tm_hour;
  counter_[DAY]    = tm.tm_mday % DAY_NUM;
}

ClockMimic::ClockMimic():instant_one_second_(NULL)
{
  for (int i = 0; i != SIZE; i++)
  {
    cur_hand_[i] = NULL;
    counter_[i] = 0;
  }
}
void ClockMimic::Resize(list<MonitorType*> list_monitor, std::size_t size)
{
  while (list_monitor.size() > size)
  {
    MonitorType *mt = list_monitor.back();
    list_monitor.pop_back();
    delete mt;
  }
}

bool ClockMimic::IsRewindSecond(struct tm& time_struct)
{
  return counter_[SECOND] > time_struct.tm_sec;
}
bool ClockMimic::IsRewindMin(struct tm& time_struct)
{
  return counter_[MINUTE] > time_struct.tm_min;
}
bool ClockMimic::IsRewindHour(struct tm& time_struct)
{
  return counter_[HOUR] > time_struct.tm_hour;
}
bool ClockMimic::IsRewindDay(struct tm& time_struct)
{
  return counter_[DAY] > (time_struct.tm_yday % DAY_NUM);
}

/*
 * sleep at least one seconds
 */
void ClockMimic::Update(tm& time_struct)
{
  MonitorType* cur_monitor = instant_one_second_;

  cur_hand_[SECOND]->Add(cur_monitor);
  last_60_seconds_.push_back(cur_monitor);
  Resize(last_60_seconds_, 60);
  instant_one_second_ = cur_monitor->Create();

  if (IsRewindSecond(time_struct))
  {
    cur_hand_[MINUTE]->Add(cur_hand_[SECOND]);
    last_60_minutes_.push_front(cur_hand_[SECOND]);
    Resize(last_60_minutes_, MINUTE_NUM);
    cur_hand_[SECOND] = cur_hand_[SECOND]->Create();
  }

  if (IsRewindMin(time_struct))
  {
    cur_hand_[HOUR]->Add(cur_hand_[MINUTE]);
    last_24_hours_.push_front(cur_hand_[MINUTE]);
    Resize(last_24_hours_, HOUR_NUM);
    cur_hand_[MINUTE] = cur_hand_[MINUTE]->Create();
  }

  if (IsRewindHour(time_struct))
  {
    cur_hand_[DAY]->Add(cur_hand_[HOUR]);
    last_24_hours_.push_front(cur_hand_[HOUR]);
    Resize(last_10_days_, DAY_NUM);
    cur_hand_[HOUR] = cur_hand_[HOUR]->Create();
  }

  //update counter_;
  counter_[SECOND] = time_struct.tm_sec;
  counter_[MINUTE] = time_struct.tm_min;
  counter_[HOUR]   = time_struct.tm_hour;
  counter_[DAY]    = time_struct.tm_mday % DAY_NUM;
}

void ClockMimic::Incr(int64_t val)
{
  instant_one_second_->Incr(val);
}

const int BUFF_SIZE = 2048;
const int LINE_COUNT = 10;
string ListFormat(list<MonitorType*> list_mt)
{
  char buffer[BUFF_SIZE] = {0};
  int pos = 0, count = 0;
  for (list<MonitorType*>::iterator it = list_mt.begin(); it != list_mt.end(); ++it)
  {
    pos += snprintf(buffer, BUFF_SIZE - pos, "%ld ", (*it)->Value());
    count++;
    if (count % LINE_COUNT == 0)
    {
      pos += snprintf(buffer, BUFF_SIZE - pos, "<br>");
    }
  }
  return string(buffer);
}

string ClockMimic::ToString()
{
  string result;
  result.append("last_60_seconds:");
  result.append(ListFormat(last_60_seconds_));
  result.append("<br>");
  result.append("last_60_minutes:");
  result.append(ListFormat(last_60_minutes_));
  result.append("last_24_hours:");
  result.append(ListFormat(last_24_hours_));
  result.append("last_10_days:");
  result.append(ListFormat(last_10_days_));
  return result;
}
void ClearMonitorList(list<MonitorType*> list_mt)
{
  while (!list_mt.empty())
  {
    MonitorType* mt = list_mt.back();
    list_mt.pop_back();
    delete mt;
  }
}
ClockMimic::~ClockMimic()
{
  delete instant_one_second_;
  for (int i = 0; i != SIZE; ++i)
  {
    delete cur_hand_[i];
  }
  ClearMonitorList(last_60_seconds_);
  ClearMonitorList(last_60_minutes_);
  ClearMonitorList(last_24_hours_);
  ClearMonitorList(last_10_days_);
}

void CopyMonitorList(const list<MonitorType*>& src, list<MonitorType*>& dest)
{
  for (list<MonitorType*>::const_iterator it = src.begin(); it != src.end(); ++it)
  {
    MonitorType *r = (*it)->Create();
    r->Add(*it);
    dest.push_back(r);
  }
}
ClockMimic::ClockMimic(const ClockMimic& other)
{
  for (int i = 0; i != SIZE; i++)
  {
    counter_[i] = other.counter_[i];
    MonitorType* mt = (other.cur_hand_[i])->Create();
    mt->Add(other.cur_hand_[i]);
    cur_hand_[i] = mt;
  }
  MonitorType* mt = (other.instant_one_second_)->Create();
  mt->Add(other.instant_one_second_);
  instant_one_second_ = mt;
  CopyMonitorList(other.last_60_seconds_, last_60_seconds_);
  CopyMonitorList(other.last_60_minutes_, last_60_minutes_);
  CopyMonitorList(other.last_24_hours_, last_24_hours_);
  CopyMonitorList(other.last_10_days_, last_10_days_);
}
void ClockMimic::operator=(const ClockMimic& other)
{
 for (int i = 0; i != SIZE; i++)
  {
    counter_[i] = other.counter_[i];
    MonitorType* mt = (other.cur_hand_[i])->Create();
    mt->Add(other.cur_hand_[i]);
    cur_hand_[i] = mt;
  }
  MonitorType* mt = (other.instant_one_second_)->Create();
  mt->Add(other.instant_one_second_);
  instant_one_second_ = mt;
  CopyMonitorList(other.last_60_seconds_, last_60_seconds_);
  CopyMonitorList(other.last_60_minutes_, last_60_minutes_);
  CopyMonitorList(other.last_24_hours_, last_24_hours_);
  CopyMonitorList(other.last_10_days_, last_10_days_);
}

Monitor* Monitor::instance_ = NULL;
pthread_mutex_t Monitor::mutex_ = PTHREAD_MUTEX_INITIALIZER;

Monitor::Monitor(): is_running_(true)
{
  pthread_t id;
  pthread_create(&id, NULL, Monitor::UpdateWrapper, this);
}

Monitor& Monitor::Instance()
{
  if (NULL == instance_)
  {
    MutexGuard mg(&mutex_);
    if (NULL == instance_)
    {
      instance_ = new Monitor();
    }
  }
  return *instance_;
}

bool Monitor::CreateClock(const string& key,  mon_type mt)
{
  MonitorType* monitor_pointer = NULL;
  if (mt == AVERAGE)
  {
    monitor_pointer = new AverageMonitor();
  }
  else
  {
    monitor_pointer = new CounterMonitor();
  }
  ClockMimic cm(monitor_pointer);
  clock_iterator ci;
  bool ret = monitor_data_.insert(ci, key);
  ci->second = cm;
  return ret;
}

bool Monitor::Incr(const string& key, int64_t val)
{
  clock_iterator ci;
  if (monitor_data_.find(ci, key))
  {
    ci->second.Incr(val);
    return true;
  }
  return false;
}
bool Monitor::Set(const string& key, const string& val)
{
  kv_iterator kvi;
  bool ret = key_val_.insert(kvi, key);
  kvi->second = val;
  return ret;
}

int32_t USECONDS = 1000000;
void Monitor::Update()
{
  while (is_running_)
  {
    usleep(1000000);
    struct tm time_struct;
    time_t cur = time(NULL);
    localtime_r(&cur, &time_struct);
    for (concurrent_hash_map<string, ClockMimic>::iterator ci = monitor_data_.begin();
         ci != monitor_data_.end(); ++ci)
    {
      ci->second.Update(time_struct);
    }
  }
}

void* Monitor::UpdateWrapper(void* monitor_pointer)
{
  Monitor* m = static_cast<Monitor*>(monitor_pointer);
  m->Update();
}
}
}
