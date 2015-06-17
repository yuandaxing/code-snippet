#include "monitor.h"
namespace MicroAd
{
namespace Utils
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

MonitorType* AverageMonitor::Create()
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

QpsMonitor::QpsMonitor(): count_(0)
{
}
void QpsMonitor::Incr(int64_t v)
{
  count_ += v;
}
QpsMonitor* QpsMonitor::Create()
{
  return new QpsMonitor();
}
void QpsMonitor::Add(MonitorType *mt)
{
  QpsMonitor* qm = dynamic_cast<QpsMonitor*>(mt);
  if (NULL == qm)
  {
    throw std::runtime_error("not QpsMonitor type");
  }
  count_ += qm->count_;
}

struct Calendar::Calendar(MonitorType* mt)
{
  if (NULL == mt)
  {
    throw std::runtime_error("monitorType NULL");
  }
  for (std::size_t i = 0; i != SIZE; ++i)
  {
    cur[i] == mt->Create();
  }
  delete mt;
}

void Calendar::Resize(list<MonitorType*> list_monitor, std::size_t size)
{
  while (list_monitor.size() > size)
  {
    MonitorType *mt = list_monitor.front();
    list_monitor.pop_front();
    delete mt;
  }
}

void Update(const  t)
{
  struct tm;
  gmtime_r(&t, &tm);
  MonitorType* cur_monitor = cur_[SECOND];
  cur_[SECOND] = cur_monitor.Create();
  last_60_seconds_.push_back(cur_monitor);
  Resize(last_60_seconds_, 60);
  cur_[MINUTE]->Add(cur_monitor);
  cur_monitor = cur_[MINUTE];

  if (tm.gmt_sec == 0)
  {
    last_60_minutes_.push_back(cur_monitor);
    Resize(last_60_minutes_, 60);
    cur_monitor = cur_[MINUTE];
    cur_[MINUTE] = cur_monitor.Create();
  }

  if (tm.gmt_min == 0)
  {

  }
}


}
}
