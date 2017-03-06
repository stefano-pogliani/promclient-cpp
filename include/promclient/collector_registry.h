// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_COLLECTOR_REGISTRY_H_
#define PROMCLIENT_COLLECTOR_REGISTRY_H_

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "promclient/collector.h"

namespace promclient {

  //! Registry of collectors to pull metrics from.
  class CollectorRegistry {
   public:
    //! Add a Collector to the regisrty.
    void Register(CollectorRef collector);

    //! Remove an existing collector from the registry.
    void Unregister(CollectorRef collector);

   protected:
    //! Thread safe access to the registry.
    std::mutex mutex_;

    //! Keep track of registered collectors.
    std::vector<CollectorRef> collectors_;

    //! Keep track of metric type by name.
    std::map<std::string, std::string> metrics_type_;

    //! Keep track of metric labels by name.
    std::map<std::string, std::set<std::string>> metrics_labels_;
  };

}  // namespace promclient

#endif  // PROMCLIENT_COLLECTOR_REGISTRY_H_
