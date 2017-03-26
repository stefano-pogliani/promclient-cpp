// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_COLLECTOR_REGISTRY_H_
#define PROMCLIENT_COLLECTOR_REGISTRY_H_

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "promclient/collector.h"

namespace promclient {

  //! Registry of collectors to pull metrics from.
  class CollectorRegistry {
   public:
    //! Returns the default registry.
    static CollectorRegistry* Default();

    enum CollectStrategy {
      SORTED = 0
    };

   public:
    //! Returns a list of metrics form all registered collectors.
    /*!
     * This methods supports collection strategies.
     * By default the `sorted` strategy is used.
     *
     * Available strategies are:
     *
     *   * `sorted`:
     *      Metrics are returned sorted by name.
     *      If two or more collectors returns metrics with the
     *      same name the samples collected are merged into one
     *      metric.
     *      Samples are sorted by role and then labels.
     *      Since the registry only allows collectors to share a
     *      metric name only if the descriptors are identical,
     *      the first descriptor for the metric is attached to the
     *      samples (irrelevant details for most cases).
     */
    MetricsList collect(
        CollectorRegistry::CollectStrategy strategy =
          CollectorRegistry::CollectStrategy::SORTED
    );

    //! Add a Collector to the regisrty.
    /*!
     * The missing `e` in `registr` is to avoid clashes
     * with the C keyword `register`.
     */
    void registr(CollectorRef collector);

    //! Remove an existing collector from the registry.
    bool unregister(CollectorRef collector);

   protected:
    //! Thread safe access to the registry.
    std::mutex mutex_;

    //! Keep track of registered collectors.
    std::vector<CollectorRef> collectors_;

    //! Keep track of metric hash by name.
    std::map<std::string, std::size_t> metrics_hash_;

    //! Implements the sorted collection strategy.
    MetricsList sortedCollect();
  };

}  // namespace promclient

#endif  // PROMCLIENT_COLLECTOR_REGISTRY_H_
