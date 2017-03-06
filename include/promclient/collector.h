// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_COLLECTOR_H_
#define PROMCLIENT_COLLECTOR_H_

#include <memory>
#include <vector>

#include "promclient/metric.h"

namespace promclient {

  //! Abstract metric collector.
  class Collector {
   public:
    virtual ~Collector() = default;

    //! Return zero or more collected metrics.
    virtual std::vector<Metric> Collect() = 0;

    //! Returns zero or more metric descriptors.
    virtual std::vector<Descriptor> Describe() = 0;
  };

  //! Skip dealing with memory allocation directly.
  typedef std::shared_ptr<Collector> CollectorRef;

}  // namespace promclient

#endif  // PROMCLIENT_COLLECTOR_H_
