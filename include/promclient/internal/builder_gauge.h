// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_BUILDER_GAUGE_H_
#define PROMCLIENT_INTERNAL_BUILDER_GAUGE_H_

#include "promclient/gauge.h"
#include "promclient/internal/builder.h"

namespace promclient {

  //! Builder for gauqes and labelled gauges.
  typedef
    promclient::internal::SimpleBuilder<Gauge, LabelledGauge>
    GaugeBuilder;

}  // namespace promclient

#endif  // PROMCLIENT_INTERNAL_BUILDER_GAUGE_H_
