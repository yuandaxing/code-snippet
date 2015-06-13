/*
 * monitoring.h
 *
 *  Created on: Aug 30, 2011
 *  Author:     Brian Y. ZHANG
 *  Email:      brianzhang@mediav.com
 */

#ifndef _BrianZ_MONITORING_H_
#define _BrianZ_MONITORING_H_
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <deque>
#include <list>
#include <memory>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>
#include "mvnode/mv_node_manager.h"
#include "StatisticsMonitorService.h"
#include "release_info_service_handler.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TNonblockingServer.h>
#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include "param_info_service_handler.h"
#include <time.h>
#include "util/MVLog.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;
using namespace statistics_info;
using std::vector;
using std::list;
using std::string;
using std::deque;

namespace monitoring
{
  namespace ModuleName
  {
    static const std::string ADSERVER = "adserver";
    static const std::string ADSELECTOR = "adselector";
    static const std::string INVENTORYSEARCH = "isearch";
    static const std::string STRIDER = "strider";
    static const std::string DSPSERVER = "dspserver";
    static const std::string ISEARCH_ROOT = "isearch_root";
    static const std::string ISEARCH_LEAF = "isearch_leaf";
    static const std::string ADSELECTOR_KEYWORD = "adselector_keyword";
    static const std::string MOBPROXY = "mobproxy";
    static const std::string EXCHANGE_READER = "reader_ex";
    static const std::string EXCHANGE = "exchange";
    static const std::string TKSERVER = "tkserver";
    static const std::string ADXPROXY = "adxproxy";
    static const std::string CMSERVER = "cmserver";
  }

  class ScopeMutex
  {
  public:
    ScopeMutex(pthread_mutex_t * mutex)
    {
      mutex_ = mutex;
      pthread_mutex_lock(mutex_);
    }

    ~ScopeMutex()
    {
      pthread_mutex_unlock(mutex_);
    }

  private:
    pthread_mutex_t * mutex_;
  };

  class ScopeSpinlock
  {
  public:
    ScopeSpinlock(pthread_spinlock_t * lock)
    {
      lock_ = lock;
      pthread_spin_lock(lock_);
    }

    ~ScopeSpinlock()
    {
      pthread_spin_unlock(lock_);
    }

  private:
    pthread_spinlock_t * lock_;
  };

  class ScopeRwlock
  {
  public:
    ScopeRwlock(pthread_rwlock_t * lock, bool is_readlock)
    {
      lock_ = lock;
      if (is_readlock)
      {
        pthread_rwlock_rdlock(lock_);
      }
      else
      {
        pthread_rwlock_wrlock(lock_);
      }
    }

    ~ScopeRwlock()
    {
      pthread_rwlock_unlock(lock_);
    }

  private:
    pthread_rwlock_t * lock_;
  };

  //-----------------------------------------------------------------------------------------------------------------

  static uint32_t S_subgroup_lengths[4] = { 60, 60, 24, 10 }; // 60, 60, 24, 10
  class UnitData
  {
  public:
    enum Constants
    {
      NONE = 0,
      QPS = 1,
      LATENCY = 2,
      SIZE = 3
    };

    UnitData() :
      total_(0), counter_(0)
    {
    }
    virtual ~UnitData() { };

    virtual void set(int64_t val)
    {
      total_ = val;
      ++counter_;
    }
    virtual void incr(int64_t val)
    {
      total_ += val;
      ++counter_;
    }

    virtual void decr(int64_t val)
    {
      total_ -= val;
      ++counter_;
    }

    int64_t total() const
    {
      return total_;
    }

    uint64_t counter() const
    {
      return counter_;
    }

    int64_t average() const
    {
      return counter_ ? (total_ / counter_) : 0;
    }

    virtual void clear()
    {
      total_ = counter_ = 0;
    }

    virtual int64_t data() const
    {
      return -1;
    }

    virtual std::string toString()
    {
      return "";
    }
    virtual std::string maxToString()
    {
      return "";
    }
    virtual std::string minToString()
    {
      return "";
    }
    virtual std::string avgToString()
    {
      return "";
    }
    int64_t total_;
    uint64_t counter_;
  };

  class QpsUnitData: public UnitData
  {
  public:
    QpsUnitData& operator+=(const QpsUnitData& another)
    {
      total_ += another.total_;
      counter_ += another.counter_;
      return *this;
    }

    QpsUnitData operator+(const QpsUnitData& another)
    {
      QpsUnitData result = *this;
      return (result += another);
    }

    std::string toString() const
    {
      std::stringstream ss;
      ss << std::setw(10) << total_;
      return ss.str();
    }

    virtual int64_t data() const
    {
      return total();
    }
  };

  class LatencyUnitData: public UnitData
  {
  public:
    LatencyUnitData& operator+=(const LatencyUnitData& another)
    {
      total_ += another.total_;
      counter_ += another.counter_;
      return *this;
    }

    LatencyUnitData& operator+(const LatencyUnitData& another)
    {
      LatencyUnitData result = *this;
      return (result += another);
    }

    std::string toString() const
    {
      std::stringstream ss;
      ss << std::setw(10) << (counter_ ? total_ / counter_ : 0);
      return ss.str();
    }

    virtual int64_t data() const
    {
      return average();
    }
  };

  // set CalendarDataGroup's periodical_reset as false when using this type
  class SizeUnitData: public UnitData
  {
    // FIXME: if N threads use a single UD, the min_ may become -(N-1)
  public:
    SizeUnitData(int64_t max = -1, int64_t min = -1): max_(max), min_(min)
    {
    };
    virtual void set(int64_t val)
    {
      if (max_ < 0 || val >= max_ || !counter_)
        max_ = val;
      if (min_ < 0 || val <= min_ || !counter_)
        min_ = val;
      if (val > 0)
        total_ += val;
        counter_++;
    }
    virtual void clear()
    {
      UnitData::clear();
      max_ = min_ = -1;
    }
    SizeUnitData& operator+=(const SizeUnitData& another)
    {
      if (another.max_ < 0 || another.min_ < 0 || another.total_ < 0 || !another.counter_)
      {
        return *this;
      }
      if (another.max_ >= max_ || !counter_)
        max_ = another.max_;
      if (another.min_ <= min_ || !counter_)
        min_ = another.min_;
      total_ += another.total_;
      counter_ += another.counter_;
      return *this;
    }
    SizeUnitData& operator+(const SizeUnitData& another)
    {
      SizeUnitData result = *this;
      return (result += another);
    }
    virtual void incr(int64_t delta)
    {
      counter_++;
      total_ += counter_ * delta;
      if (counter_ > 0 && int64_t(total_ / counter_) > max_)
        max_ = total_ / counter_;
    }
    virtual void decr(int64_t delta)
    {
      counter_++;
      total_ -= counter_ * delta;
      if (counter_ > 0 && int64_t(total_ / counter_) < min_)
        min_ = total_ / counter_;
    }

    virtual int64_t data() const
    {
      return average();
    }

    virtual std::string toString() const
    {
      std::stringstream ss;
      ss << std::setw(30) << "(" << min_ << "," << int(counter_? total_ / counter_ : -1)  << "," << max_ << ")";
      return ss.str();
    }
    std::string maxToString() const
    {
      std::stringstream ss;
      ss << std::setw(10) << max_ ;
      return ss.str();
    }
    std::string minToString() const
    {
      std::stringstream ss;
      ss << std::setw(10) << min_ ;
      return ss.str();
    }
    std::string avgToString() const
    {
      std::stringstream ss;
      ss << std::setw(10) << int(counter_? total_ / counter_ : -1);
      return ss.str();
    }
    int64_t max_;
    int64_t min_;
  };

  class DataGroupIf
  {
  public:
    virtual ~DataGroupIf()
    {
    }

    virtual void incr(int64_t delta) = 0;
    virtual void decr(int64_t delta) = 0;
    virtual void set(int64_t delta) = 0;
    virtual void recompute() = 0;
    virtual std::string toString() const = 0;

    virtual int64_t getLastMiniuteData() const = 0;
    virtual void getLast60MinutesData(vector<int64_t>& data_list) = 0;
    virtual void getLast60SecondsData(vector<int64_t>& data_list) = 0;

    // for flow control
    virtual int64_t getLastSecondsData(uint32_t s=10) = 0;
    virtual int64_t getLastMinutesData(uint32_t m=10) = 0;
  };

  template<typename UD>
  class CalendarDataGroup: public DataGroupIf
  {
  public:
    typedef deque<UD> SubDataGroup;
    enum Type
    {
        SECOND = 0,
        MINUTE = 1,
        HOUR = 2,
        DAY = 3,
        NA = 4
    };

    CalendarDataGroup(bool periodical_reset = true)
    {
      time_t t = time(NULL);
      struct tm broken;
      localtime_r(&t, &broken);
      counters_[0] = broken.tm_sec % S_subgroup_lengths[0];
      counters_[1] = broken.tm_min % S_subgroup_lengths[1];
      counters_[2] = broken.tm_hour % S_subgroup_lengths[2];
      counters_[3] = broken.tm_mday % S_subgroup_lengths[3];// somewhat useless, bcuz there is no upper level counter
      time_items_.resize(4);
      current_items_.resize(4);
      for (int32_t group = 0; group < 4; ++group)
      {
        time_items_[group].resize(S_subgroup_lengths[group]);
      }
      current_items_[0].clear();
      periodical_reset_ = periodical_reset;
      pthread_mutex_init(&uniq_lock_,NULL);
    }

    virtual ~CalendarDataGroup()
    {

    }

    void incr(int64_t delta)
    {
      ScopeMutex smx(&uniq_lock_);
      current_items_[0].incr(delta);
    }

    void decr(int64_t delta)
    {
      ScopeMutex smx(&uniq_lock_);
      current_items_[0].decr(delta);
    }

    void set(int64_t val)
    {
      ScopeMutex smx(&uniq_lock_);
      current_items_[0].set(val);
    }
    virtual std::string toString() const
    {
      ScopeMutex smx(const_cast<pthread_mutex_t *> (&uniq_lock_));
      size_t width = 10;

      std::stringstream ss;

      std::string names[4] = { std::string("last 60 sec:"), std::string("last 60 min:"), std::string("last 24 hour:"),
          std::string("last 10 days:") };

      for (int32_t type = 0; type < 4; ++type)
      {
        ss << names[type] << "\n";
        uint32_t subgroup_count = 0;
        for (typename SubDataGroup::const_iterator it = time_items_[type].begin();
            it != time_items_[type].end(); ++it, ++subgroup_count)
        {
          ss << ' ' << it->toString();
          if ((subgroup_count == S_subgroup_lengths[type] - 1) || ((subgroup_count + 1) % width) == 0)
          {
            ss << "\n";
          }
        }
      }
      return ss.str();
    }

    virtual int64_t getLastMiniuteData() const
    {
      return time_items_[MINUTE][0].data();
    }

    virtual void getLast60MinutesData(vector<int64_t>& data_list)
    {
      data_list.resize(60, 0);
      uint32_t subgroup_idx = 0;
      for (typename SubDataGroup::const_iterator it = time_items_[MINUTE].begin();
          it != time_items_[MINUTE].end() && subgroup_idx < 60; ++it, ++subgroup_idx)
      {
        data_list[subgroup_idx] = it->data();
      }
    }

    virtual void getLast60SecondsData(vector<int64_t>& data_list)
    {
      data_list.resize(60, 0);
      uint32_t subgroup_idx = 0;
      for (typename SubDataGroup::const_iterator it = time_items_[SECOND].begin();
          it != time_items_[SECOND].end() && subgroup_idx < 60; ++it, ++subgroup_idx)
      {
        data_list[subgroup_idx] = it->data();
      }
    }

    // s must be in (0, 60]
    virtual int64_t getLastSecondsData(uint32_t s)
    {
      int64_t sum = 0;
      uint32_t subgroup_idx = 0;
      for (typename SubDataGroup::const_iterator it = time_items_[SECOND].begin();
          it != time_items_[SECOND].end() && subgroup_idx < s; ++it, ++subgroup_idx)
      {
        sum += it->data();
      }
      return sum / s;
    }

    // m must be in (0, 60]
    virtual int64_t getLastMinutesData(uint32_t m)
    {
      int64_t sum = 0;
      uint32_t subgroup_idx = 0;
      for (typename SubDataGroup::const_iterator it = time_items_[MINUTE].begin();
          it != time_items_[MINUTE].end() && subgroup_idx < m; ++it, ++subgroup_idx)
      {
        if (it->data() == 0)
        {
          break;
        }
        sum += it->data();
      }
      return sum / m;
    }

    virtual void recompute()
    {
      recompute(time(NULL));
    }

    virtual void recompute(time_t t)
    {
      ScopeMutex smx(&uniq_lock_);
      updateCounters(t);
      recomputeSeconds();
      recomputeMinutes();
      recomputeHours();
      recomputeDays();
      if (periodical_reset_)
      {
        current_items_[0].clear();
      }
    }

    int get(UD& result, Type type, uint32_t offset = 0)
    {
      if (type < 4)
      {
        if (offset < S_subgroup_lengths[type])
        {
          result = time_items_[type][offset];
        }
      }
      return -1;
    }

    virtual void updateCounters(time_t t)
    {
      struct tm broken;
      localtime_r(&t, &broken);

      if (broken.tm_sec == 0)
      {
        counters_[0] = 0;
      }
      else
      {
        counters_[0] = (counters_[0] + 1) % S_subgroup_lengths[0];
      }
      counters_[1] = broken.tm_min % S_subgroup_lengths[1];
      counters_[2] = broken.tm_hour % S_subgroup_lengths[2];
      counters_[3] = broken.tm_mday % S_subgroup_lengths[3];

      current_items_[MINUTE] += current_items_[SECOND];
      current_items_[HOUR] += current_items_[SECOND];
      current_items_[DAY] += current_items_[SECOND];
    }

    void recomputeSeconds()
    {
      time_items_[SECOND].pop_back();
      time_items_[SECOND].push_front(current_items_[SECOND]);
    }

    void recomputeMinutes()
    {
      if (counters_[SECOND] == 0)               // carry, update
      {
        time_items_[MINUTE].push_front(current_items_[MINUTE]);
        time_items_[MINUTE].pop_back();
        current_items_[MINUTE].clear();
      }
    }

    void recomputeHours()
    {
      if (counters_[SECOND] || counters_[MINUTE])
      {
        time_items_[HOUR][0] = current_items_[HOUR];
      }
      else
      {
        time_items_[HOUR].push_front(current_items_[HOUR]);
        time_items_[HOUR].pop_back();
        current_items_[HOUR].clear();
      }
    }

    void recomputeDays()
    {
      if (counters_[SECOND] || counters_[MINUTE] || counters_[HOUR])
      {
        time_items_[DAY][0] = current_items_[DAY];
      }
      else
      {
        time_items_[DAY].push_front(current_items_[DAY]);
        time_items_[DAY].pop_back();
        current_items_[DAY].clear();
      }
    }

  //protected:
    int32_t counters_[4];
    vector<SubDataGroup> time_items_;
    vector<UD> current_items_;
    bool periodical_reset_;
    pthread_mutex_t uniq_lock_;
  };

  template<typename UD>
  class HistogramDataGroup: public CalendarDataGroup<UD>
  {
  public:
    typedef deque<UD> SubDataGroup;
    using CalendarDataGroup<UD>::uniq_lock_;
    using CalendarDataGroup<UD>::time_items_;
    HistogramDataGroup():CalendarDataGroup<UD>()
    {

    }
    std::string toString() const
    {
      ScopeMutex smx(const_cast<pthread_mutex_t *> (&uniq_lock_));
      size_t width = 10;

      std::stringstream ss;

      std::string names[4] = { std::string("last 60 sec"), std::string("last 60 min"), std::string("last 24 hour"),
         std::string("last 10 days:") };

      for (int32_t type = 0; type < 4; ++type)
      {
        ss << names[type] << " max "<< ":\n";
        int32_t subgroup_count = 0;
        for (typename SubDataGroup::const_iterator it = time_items_[type].begin();
            it != time_items_[type].end(); ++it, ++subgroup_count)
        {
          ss << ' ' << it->maxToString();
          if ((subgroup_count == int32_t(S_subgroup_lengths[type] - 1)) || ((subgroup_count + 1) % width) == 0)
          {
            ss << "\n";
          }
        }
        ss << names[type] << " min "<< ":\n";
        subgroup_count = 0;
        for (typename SubDataGroup::const_iterator it = time_items_[type].begin();
                  it != time_items_[type].end(); ++it, ++subgroup_count)
        {
          ss << ' ' << it->minToString();
          if ((subgroup_count == int32_t(S_subgroup_lengths[type] - 1)) || ((subgroup_count + 1) % width) == 0)
          {
            ss << "\n";
          }
        }
        ss << names[type] << " avg "<< ":\n";
        subgroup_count = 0;
        for (typename SubDataGroup::const_iterator it = time_items_[type].begin();
                it != time_items_[type].end(); ++it, ++subgroup_count)
        {
          ss << ' ' << it->avgToString();
          if ((subgroup_count == int32_t(S_subgroup_lengths[type] - 1)) || ((subgroup_count + 1) % width) == 0)
          {
            ss << "\n";
          }
        }
      }

      return ss.str();
    }
  };

  //------alert module-----------------------------------------------------------------------------------------------------
  class AlertHelper
  {
  public:
    static void SetCapacity(uint32_t size)
    {
      alerts_.set_capacity(size);
    }
    static void Alert(std::string message, AlertLevel::type level)
    {
      AlertInfo info;
      info.message = message;
      info.level = level;
      info.time = timeToString(time(NULL));
      alerts_.try_push(info);
    }
    static bool GetAlert(std::vector<AlertInfo> & alerts)
    {
      AlertInfo info;
      int MAX_RESULT = 100;
      int count = 0;
      while (alerts_.try_pop(info) && count < MAX_RESULT)
      {
        alerts.push_back(info);
        count++;
      }

      return true;
    }

    static std::string timeToString(time_t t)
    {
      struct tm broken;
      localtime_r(&t, &broken);

      char str_time[128];
      strftime(str_time, sizeof(str_time), "%Y.%m.%d-%H:%M:%S %z %Z", &broken);

      return std::string(str_time);
    }
  private:
    static tbb::concurrent_bounded_queue<AlertInfo> alerts_;

  };
  //-----------------------------------------------------------------------------------------------------------------

  class StatisticsMonitorServiceHandler: virtual public param_info::ParamInfoServiceHandler,
    virtual public StatisticsMonitorServiceIf, virtual public ReleaseInfoServiceHandler
  {
  public:
    StatisticsMonitorServiceHandler(time_t start_time, tbb::concurrent_hash_map<std::string, DataGroupIf *> * groups,
                                    tbb::concurrent_hash_map<std::string, std::string> * str_to_str)
    {
      start_time_ = start_time;

      groups_ = groups;

      str_to_str_ = str_to_str;
    }

    void getInfo(StatisticsInfo& _return, const std::vector<std::string> & stat_names,
                 const std::vector<std::string> & string_names)
    {
      _return.now = timeToString(time(NULL));
      _return.__isset.now = true;

      _return.start_time = timeToString(start_time_);
      _return.__isset.start_time = true;

      if (stat_names.size())
      {
        for (size_t i = 0; i < stat_names.size(); ++i)
        {
          tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_accessor accessor;
          if (groups_->find(accessor, stat_names[i]))
          {
            _return.stats[stat_names[i]] = accessor->second->toString();
            _return.__isset.stats = true;
          }
        }
      }

      if (string_names.size())
      {
        for (size_t i = 0; i < string_names.size(); ++i)
        {
          tbb::concurrent_hash_map<std::string, std::string>::const_accessor accessor;
          if (str_to_str_->find(accessor, string_names[i]))
          {
            _return.strings[string_names[i]] = accessor->second;
            _return.__isset.strings = true;
          }
        }
      }
    }

    void getEverything(StatisticsInfo& _return)
    {
      _return.now = timeToString(time(NULL));
      _return.__isset.now = true;

      _return.start_time = timeToString(start_time_);
      _return.__isset.start_time = true;

      for (tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_iterator iter = groups_->begin(); iter
          != groups_->end(); ++iter)
      {
        _return.stats[iter->first] = iter->second->toString();
      }
      _return.__isset.stats = true;

      for (tbb::concurrent_hash_map<std::string, std::string>::const_iterator iter = str_to_str_->begin(); iter
          != str_to_str_->end(); ++iter)
      {
        _return.strings[iter->first] = iter->second;
      }
      _return.__isset.strings = true;

    }

    //----for alert
    void GetAlert(std::vector<AlertInfo>& _return)
    {
      AlertHelper::GetAlert(_return);
    }

    void getStartTime(std::string& _return)
    {
      _return = timeToString(start_time_);
    }

    virtual void getLastMinuteStatus(LastMinStatus& _return,
      const std::vector<std::string> & numerical_item_names, const std::vector<std::string> & string_item_names)
    {
      // get numerical data
      _return.__isset.last_minute_numerical_status = true;
      _return.__isset.last_minute_string_status = true;

      if(numerical_item_names.size() == 0)
      {
        for (tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_iterator iter = groups_->begin(); iter
            != groups_->end(); ++iter)
        {
          _return.last_minute_numerical_status[iter->first] = iter->second->getLastMiniuteData();
        }
      }
      else
      {
        for (std::vector<std::string>::const_iterator citer = numerical_item_names.begin();
            citer != numerical_item_names.end(); ++citer)
        {
          tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_accessor accessor;
          if (groups_->find(accessor, *citer))
          {
            _return.last_minute_numerical_status[*citer] = accessor->second->getLastMiniuteData();
          }
        }
      }

      // get string data
      _return.__isset.last_minute_string_status = true;
      if(string_item_names.size() == 0)
      {
        for (tbb::concurrent_hash_map<std::string, std::string>::const_iterator iter = str_to_str_->begin(); iter
            != str_to_str_->end(); ++iter)
        {
          _return.last_minute_string_status[iter->first] = iter->second;
        }
      }
      else
      {
        for (std::vector<std::string>::const_iterator citer = string_item_names.begin();
            citer != string_item_names.end(); ++citer)
        {
          tbb::concurrent_hash_map<std::string, std::string>::const_accessor accessor;
          if (str_to_str_->find(accessor, *citer))
          {
            _return.last_minute_string_status[*citer] = accessor->second;
          }
        }
      }

    }

    virtual void getLast60MinutesData(Last60MinuteData& _return,
      const std::vector<std::string> & numerical_item_names)
    {
      _return.clear();

      if(numerical_item_names.size() == 0)
      {
        for (tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_iterator iter = groups_->begin(); iter
            != groups_->end(); ++iter)
        {
          std::vector<int64_t>& data_list = _return[iter->first];
          iter->second->getLast60MinutesData(data_list);
        }
      }
      else
      {
        for (std::vector<std::string>::const_iterator citer = numerical_item_names.begin();
            citer != numerical_item_names.end(); ++citer)
        {
          tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_accessor accessor;
          if (groups_->find(accessor, *citer))
          {
            std::vector<int64_t>& data_list = _return[*citer];
            accessor->second->getLast60MinutesData(data_list);
          }
        }
      }
    }

    virtual void getLast60SecondsData(Last60SecondData& _return,
      const std::vector<std::string> & numerical_item_names)
    {
      _return.clear();

      if(numerical_item_names.size() == 0)
      {
        for (tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_iterator iter = groups_->begin(); iter
            != groups_->end(); ++iter)
        {
          std::vector<int64_t>& data_list = _return[iter->first];
          iter->second->getLast60SecondsData(data_list);
        }
      }
      else
      {
        for (std::vector<std::string>::const_iterator citer = numerical_item_names.begin();
            citer != numerical_item_names.end(); ++citer)
        {
          tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_accessor accessor;
          if (groups_->find(accessor, *citer))
          {
            std::vector<int64_t>& data_list = _return[*citer];
            accessor->second->getLast60SecondsData(data_list);
          }
        }
      }
    }

  protected:
    std::string timeToString(time_t t)
    {
      struct tm broken;
      localtime_r(&t, &broken);

      char str_time[128];
      strftime(str_time, sizeof(str_time), "%Y.%m.%d-%H:%M:%S %z %Z", &broken);

      return std::string(str_time);
    }

  private:
    time_t start_time_;

    tbb::concurrent_hash_map<std::string, DataGroupIf *> * groups_;
    tbb::concurrent_hash_map<std::string, std::string> * str_to_str_;
  };

  class Statistics
  {
    typedef std::map<std::string, std::string> MonitoringMap;
  public:
    struct UpdaterArg
    {
      UpdaterArg(uint32_t intv = default_update_interval_ms_) :
        update_interval_ms_(intv)
      {

      }

      uint32_t update_interval_ms_;

      static const uint32_t default_update_interval_ms_ = 1000;
    };

    struct ListenerArg
    {
      ListenerArg(uint16_t pn = default_port_number_) :
        port_number_(pn)
      {
      }

      uint16_t port_number_;

      static const uint16_t default_port_number_ = 7828;
    };

    static Statistics * getInstance(uint16_t port = ListenerArg::default_port_number_)
    {
      if (!uniq_instance_)
      {
        ScopeMutex sm(&uniq_instance_lock_);

        if (!uniq_instance_)
        {
          Statistics * temp = new Statistics();

          listener_arg_.port_number_ = port;

          {
            tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor acc1;
            groups_.insert(acc1, "qps");
            acc1->second = new CalendarDataGroup<QpsUnitData> ();
          }

          {
            tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor acc2;
            groups_.insert(acc2, "latency");
            acc2->second = new CalendarDataGroup<LatencyUnitData> ();
          }

          pthread_create(&updater_tid_, NULL, updaterStartRoutine, &updater_arg_);
          pthread_create(&listener_tid_, NULL, listenerStartRoutine, &listener_arg_);

          uniq_instance_ = temp;
          auto_ = std::auto_ptr<Statistics>(temp);
          AlertHelper::SetCapacity(1000);
          int wait = 0;
          //等待thrift service 初始化
          while(!ready_)
          {
            sleep(1);
            if(wait > 2)
            {
              break;
            }
            wait ++;
          }
        }
      }

      return uniq_instance_;
    }

    static void destroyInstance()
    {
      if (listener_server_)
      {
        delete listener_server_;
        listener_server_ = NULL;
      }

      for (tbb::concurrent_hash_map<std::string, DataGroupIf *>::iterator iter = groups_.begin(); iter != groups_.end(); ++iter)
      {
        delete iter->second;
      }
      groups_.clear();
    }

    void stop()
    {
      stopUpdater();
      stopListener();
      joinUpdater();
      joinListener();
      if (listener_server_)
      {
        delete listener_server_;
        listener_server_ = NULL;
      }

      for (tbb::concurrent_hash_map<std::string, DataGroupIf *>::iterator iter = groups_.begin(); iter != groups_.end(); ++iter)
      {
        delete iter->second;
      }
      groups_.clear();
      ready_ = false;
    }

    // 0 if ok, -1 if duplicate
    int registerNewDataGroup(const std::string& name, DataGroupIf * group)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (!groups_.find(accessor, name))
      {
        groups_.insert(accessor, name);
        accessor->second = group;
        return 0;
      }
      else
      {
        return -1;
      }
    }

    int removeDataGroup(const std::string& name)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        delete accessor->second;
        groups_.erase(accessor);
        return 0;
      }

      return -1;
    }

    int set(const std::string& name, int64_t val)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        accessor->second->set(val);
        return 0;
      }
      else
      {
        return -1;
      }
    }

    bool getLastSecondsLatency(const std::string& name, uint32_t s, int64_t& latency)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        latency = accessor->second->getLastSecondsData(s);
        return true;
      }
      else
      {
        return false;
      }
    }

    bool getLastMinutesLatency(const std::string& name, uint32_t m, int64_t& latency)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        latency = accessor->second->getLastMinutesData(m);
        return true;
      }
      else
      {
        return false;
      }
    }

    int incr(const std::string& name, int64_t delta)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        accessor->second->incr(delta);
        return 0;
      }
      else
      {
        return -1;
      }
    }

    int decr(const std::string& name, int64_t delta)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        accessor->second->decr(delta);
        return 0;
      }
      else
      {
        return -1;
      }
    }

    int setOrInit(const std::string& name, int64_t val, UnitData::Constants type = UnitData::SIZE)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        accessor->second->set(val);
        return 0;
      }
      else
      {
        groups_.insert(accessor, name);
        switch (type)
        {
          case UnitData::QPS:
            accessor->second = new CalendarDataGroup<QpsUnitData> ();
            accessor->second->set(val);
            return 0;
            break;
          case UnitData::LATENCY:
            accessor->second = new CalendarDataGroup<LatencyUnitData> ();
            accessor->second->set(val);
            return 0;
            break;
          case UnitData::SIZE:
            accessor->second = new HistogramDataGroup<SizeUnitData>();
            accessor->second->set(val);
            return 0;
            break;
          case UnitData::NONE:
            // fall through
          default:
            // invalid or unknown type
            return -1;
            break;
        }
      }
      return -1;
    }

    int incrOrInit(const std::string& name, int64_t delta, UnitData::Constants type = UnitData::QPS)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        accessor->second->incr(delta);
        return 0;
      }
      else
      {
        groups_.insert(accessor, name);
        switch (type)
        {
          case UnitData::QPS:
            accessor->second = new CalendarDataGroup<QpsUnitData> ();
            accessor->second->incr(delta);
            return 0;
            break;
          case UnitData::LATENCY:
            accessor->second = new CalendarDataGroup<LatencyUnitData> ();
            accessor->second->incr(delta);
            return 0;
            break;
          case UnitData::SIZE:
            accessor->second = new HistogramDataGroup<SizeUnitData>();
            accessor->second->incr(delta);
            return 0;
            break;
          case UnitData::NONE:
            // fall through
          default:
            // invalid or unknown type
            return -1;
            break;
        }
      }
      return -1;
    }

    int decrOrInit(const std::string& name, int64_t delta, UnitData::Constants type = UnitData::QPS)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        accessor->second->decr(delta);
        return 0;
      }
      else
      {
        groups_.insert(accessor, name);
        switch (type)
        {
          case UnitData::QPS:
            accessor->second = new CalendarDataGroup<QpsUnitData> ();
            accessor->second->decr(delta);
            return 0;
            break;
          case UnitData::LATENCY:
            accessor->second = new CalendarDataGroup<LatencyUnitData> ();
            accessor->second->decr(delta);
            return 0;
            break;
          case UnitData::SIZE:
            accessor->second = new HistogramDataGroup<SizeUnitData>();
            accessor->second->decr(delta);
            return 0;
            break;
          case UnitData::NONE:
            // fall through
          default:
            // invalid or unknown type
            return -1;
            break;
        }
      }
    }

    bool setString(const std::string& name, const std::string& value)
    {
      tbb::concurrent_hash_map<std::string, std::string>::accessor accessor;
      str_to_str_.insert(accessor, name);
      accessor->second = value;
      return true;
    }

    bool setCurrentTimeAsString(const std::string& name)
    {
      time_t current_time = time(0);
      return setTimeAsString(name, current_time);
    }

    bool setTimeAsString(const std::string& name, time_t timestamp)
    {
      tbb::concurrent_hash_map<std::string, std::string>::accessor accessor;

      if(timestamp <= 0)
      {
        str_to_str_.insert(accessor, name);
        accessor->second = "N/A";
        return true;
      }

      struct tm newtime;
      localtime_r(&timestamp, &newtime);
      char buff[128];
      strftime(buff, sizeof(buff), "%Y.%m.%d-%H:%M:%S %z %Z", &newtime);

      str_to_str_.insert(accessor, name);
      accessor->second = buff;
      return true;
    }

    bool getString(const std::string& name, std::string& result)
    {
      tbb::concurrent_hash_map<std::string, std::string>::const_accessor const_accessor;
      if (str_to_str_.find(const_accessor, name))
      {
        result = const_accessor->second;
        return true;
      }
      else
      {
        return false;
      }

    }

    bool delString(const std::string& name)
    {
      return str_to_str_.erase(name);
    }

    std::string toString()
    {
      std::stringstream ss;

      for (tbb::concurrent_hash_map<std::string, DataGroupIf *>::iterator iter = groups_.begin(); iter != groups_.end(); ++iter)
      {
        ss << "* " << iter->first << ":\n" << iter->second->toString();
      }

      for (tbb::concurrent_hash_map<std::string, std::string>::iterator iter = str_to_str_.begin(); iter
          != str_to_str_.end(); ++iter)
      {
        ss << "* " << iter->first << ":\t" << iter->second << "\n";
      }

      return ss.str();
    }

    int getDataGroup(const std::string& name, DataGroupIf& result)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf*>::const_accessor accessor;
      if (groups_.find(accessor, name))
      {
        result = *(accessor->second);
        return 0;
      }
      else
      {
        return -1;
      }
    }

    void stopUpdater()
    {
      updater_stop_ = true;
    }

    void stopListener()
    {
      if (listener_server_)
      {
        listener_server_->stop();
        threadManager_->stop();
      }
    }

    int joinUpdater()
    {
      void * result = NULL;
      int ret = 0;
      if (updater_tid_)
      {
        ret = pthread_join(updater_tid_, &result);
        updater_tid_ = 0;
      }
      return ret;
    }

    int joinListener()
    {
      void * result = NULL;
      int ret = 0;
      if (listener_tid_)
      {
        ret = pthread_join(listener_tid_, &result);
        listener_tid_ = 0;
      }
      return ret;
    }

    bool register2ZK(const std::string& moduleName, const std::string& ip, const int port)
    {
      mvnode::MVNodeManager* mgr = mvnode::MVNodeManager::GetInstance();
      MonitoringMap::const_iterator it = monitoringMap_.find(moduleName);
      if (it != monitoringMap_.end())
      {
        if (!mgr->CreatePath(it->second))
        {
            return false;
        }
        char buffer[512] = { 0 };
        snprintf(buffer, 511, "%s:%d", ip.c_str(), port);
        return mgr->CreateZNode(it->second + "/node-", buffer, ZOO_EPHEMERAL | ZOO_SEQUENCE, monitoringPath_);
      }
      MV_DEBUG((L_ERROR, "Unknown module name: %s\n", moduleName.c_str()));
      return false;
    }

    bool removeFromZK()
    {
      mvnode::MVNodeManager* mgr = mvnode::MVNodeManager::GetInstance();
      if(!monitoringPath_.empty())
      {
        return mgr->DeletePath(monitoringPath_);
      }
      MV_DEBUG((L_ERROR, "monitoringPath_ is empty\n"));
      return false;
    }

    shared_ptr<StatisticsMonitorServiceHandler> handler()
    {
      return handler_;
    }

#if 0
    // this method is ugly!
    void typeInfo(const std::string& name)
    {
      tbb::concurrent_hash_map<std::string, DataGroupIf *>::accessor accessor;
      if (groups_.find(accessor, name))
      {
        std::cout << name << ": " << typeid(*(accessor->second)).name() << std::endl;
      }
      else
      {
        std::cout << name << ": unknown" << std::endl;
      }
    }
#endif

    //--------- for alert -------------
    void Alert(std::string message, AlertLevel::type level)
    {
      AlertHelper::Alert(message, level);
    }
  protected:
    Statistics()
    {
      // nothing
    }
    friend class std::auto_ptr<Statistics>;
    virtual ~Statistics()
    {
      stop();
    }

    static void recompute()
    {
      for (tbb::concurrent_hash_map<std::string, DataGroupIf *>::iterator iter = groups_.begin(); iter != groups_.end(); ++iter)
      {
        iter->second->recompute();
      }
    }

    static bool initMonitoringMap();

    // periodically update statistics
    static void * updaterStartRoutine(void * arg)
    {
      struct UpdaterArg * my_arg = (struct UpdaterArg *) arg;

      struct timespec sleep_interval;
      sleep_interval.tv_sec = my_arg->update_interval_ms_ / 1000;
      sleep_interval.tv_nsec = (my_arg->update_interval_ms_ % 1000) * 1000000;

#ifdef MONITORING_FRAMEWORK_DEMO_SERVER
      time_t now;
      struct tm now_broken;
      char time_str_buf[64];
#endif

      while (!updater_stop_)
      {
        nanosleep(&sleep_interval, NULL);
        recompute();
#ifdef MONITORING_FRAMEWORK_DEMO_SERVER
        time(&now);
        localtime_r(&now, &now_broken);
        strftime(time_str_buf, sizeof(time_str_buf), "%Y.%m.%d-%H:%M:%S %z %Z", &now_broken);
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "\t"
        << "current local time: " << time_str_buf << "\n"
        << uniq_instance_->toString() << std::endl;
#endif
      }

      return (void *) 0;
    }

    // serves remote RPC clients
    static void * listenerStartRoutine(void * arg)
    {
      struct ListenerArg * my_arg = (struct ListenerArg *) arg;

      time_t start_time;
      time(&start_time);
      try
      {
        handler_ = shared_ptr<StatisticsMonitorServiceHandler>(
          new StatisticsMonitorServiceHandler(start_time, &groups_, &str_to_str_));
        shared_ptr<TProcessor> processor(new StatisticsMonitorServiceProcessor(handler_));
        shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
        threadManager_ = ThreadManager::newSimpleThreadManager(1);
        shared_ptr<PosixThreadFactory> threadFactory =
            shared_ptr<PosixThreadFactory> (new PosixThreadFactory());

        threadManager_->threadFactory(threadFactory);
        threadManager_->start();

        listener_server_ = new TNonblockingServer(processor, protocolFactory, my_arg->port_number_, threadManager_);
        ready_ = true;
        listener_server_->serve();
      }
      catch (TException& e)
      {
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":Error:" << e.what() << std::endl;
      }
      catch (...)
      {
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":start monitoring error" << std::endl;
      }
      return (void *) 0;
    }

  private:
    std::string monitoringPath_;
    static pthread_mutex_t uniq_instance_lock_;
    static Statistics * uniq_instance_;
    static std::auto_ptr<Statistics> auto_;
    static MonitoringMap monitoringMap_;
    static bool initialized_;
    static bool ready_;

    static pthread_t updater_tid_;
    static pthread_t listener_tid_;
    static struct UpdaterArg updater_arg_;
    static struct ListenerArg listener_arg_;
    static shared_ptr<StatisticsMonitorServiceHandler> handler_;
    static shared_ptr<ThreadManager> threadManager_;

    static bool updater_stop_;
    static TServer * listener_server_;

    static tbb::concurrent_hash_map<std::string, DataGroupIf *> groups_;
    static tbb::concurrent_hash_map<std::string, std::string> str_to_str_;
  };
}

#endif /* _BrianZ_MONITORING_H_ */
