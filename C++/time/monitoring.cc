/*
 * monitoring.cc
 *
 *  Created on: Sep 1, 2011
 *  Author:     Brian Y. ZHANG
 *  Email:      brianzhang@mediav.com
 */

#include "monitoring.h"

namespace monitoring
{
    pthread_mutex_t Statistics::uniq_instance_lock_ = PTHREAD_MUTEX_INITIALIZER;
    Statistics * Statistics::uniq_instance_ = NULL;

    pthread_t Statistics::updater_tid_ = 0;
    pthread_t Statistics::listener_tid_ = 0;
    struct Statistics::UpdaterArg Statistics::updater_arg_;
    struct Statistics::ListenerArg Statistics::listener_arg_;
    Statistics::MonitoringMap Statistics::monitoringMap_;
    bool Statistics::initialized_ = initMonitoringMap();
    bool Statistics::ready_ = false;

    const uint32_t Statistics::UpdaterArg::default_update_interval_ms_;
    const uint16_t Statistics::ListenerArg::default_port_number_;
    std::auto_ptr<Statistics> Statistics::auto_;
    shared_ptr<ThreadManager> Statistics::threadManager_;
    shared_ptr<StatisticsMonitorServiceHandler> Statistics::handler_;
    bool Statistics::updater_stop_ = false;
    TServer * Statistics::listener_server_ = NULL;

    tbb::concurrent_hash_map<std::string, DataGroupIf *> Statistics::groups_;
    tbb::concurrent_hash_map<std::string, std::string> Statistics::str_to_str_;
    tbb::concurrent_bounded_queue<AlertInfo> AlertHelper::alerts_;

    bool Statistics::initMonitoringMap()
    {
      monitoringMap_[ModuleName::ADSELECTOR] = "/ambition/monitor/adselector";
      monitoringMap_[ModuleName::ADSERVER] = "/ambition/monitor/adserver";
      monitoringMap_[ModuleName::INVENTORYSEARCH] = "/ambition/monitor/isearch";
      monitoringMap_[ModuleName::STRIDER] = "/ambition/monitor/strider";
      monitoringMap_[ModuleName::DSPSERVER] = "/ambition/monitor/dspserver";
      monitoringMap_[ModuleName::ISEARCH_ROOT] = "/ambition/monitor/isearch_root";
      monitoringMap_[ModuleName::ISEARCH_LEAF] = "/ambition/monitor/isearch_leaf";
      monitoringMap_[ModuleName::ADSELECTOR_KEYWORD] = "/ambition/monitor/adselector_keyword";
      monitoringMap_[ModuleName::MOBPROXY] = "/ambition/monitor/mobproxy";
      monitoringMap_[ModuleName::TKSERVER] = "/ambition/monitor/tkserver";
      monitoringMap_[ModuleName::EXCHANGE] = "/ambition/monitor/exchange";
      monitoringMap_[ModuleName::ADXPROXY] = "/ambition/monitor/adxproxy";
      monitoringMap_[ModuleName::CMSERVER] = "/ambition/monitor/cmserver";
      return true;
    }
}
