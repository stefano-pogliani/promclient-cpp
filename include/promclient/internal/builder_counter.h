// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_BUILDER_COUNTER_H_
#define PROMCLIENT_INTERNAL_BUILDER_COUNTER_H_

#include "promclient/counter.h"
#include "promclient/internal/builder.h"

namespace promclient {

  //! Builder for counters and labelled counters.
  typedef
    promclient::internal::SimpleBuilder<Counter, LabelledCounter>
    CounterBuilder;

}  // namespace promclient

#endif  // PROMCLIENT_INTERNAL_BUILDER_COUNTER_H_
