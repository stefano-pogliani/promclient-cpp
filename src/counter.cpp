// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/counter.h"

#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "promclient/exceptions.h"
#include "promclient/metric.h"

using promclient::Counter;
using promclient::LabelledCollector;
using promclient::LabelledCounter;

using promclient::CounterDecrease;
using promclient::Descriptor;
using promclient::DescriptorRef;
using promclient::DescriptorsList;

using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;


Counter::Counter(std::string name, std::string help, double initial) {
  this->help_ = help;
  this->name_ = name;
  this->value_ = initial;
  this->descriptor_ = DescriptorRef(new Descriptor(
      this->name_, "counter", this->help_, {}
  ));
}

MetricsList Counter::collect() {
  double value = 0;
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    value = this->value_;
  }
  MetricsList metrics;
  Sample sample("", value, {});
  metrics.push_back(Metric(this->descriptor_, {sample}));
  return metrics;
}

DescriptorsList Counter::describe() {
  return DescriptorsList({this->descriptor_});
}

void Counter::inc(double value) {
  if (value < 0) {
    throw CounterDecrease(this->name_);
  }
  std::lock_guard<std::mutex> lock(this->mutex_);
  this->value_ += value;
}


LabelledCounter::LabelledCounter(
    std::string name, std::string help,
    std::set<std::string> labels
) : LabelledCollector<Counter>(labels) {
  this->help_ = help;
  this->name_ = name;
}

LabelledCounter::Ref LabelledCounter::makeChild() {
  return LabelledCounter::Ref(new Counter(this->name_, this->help_));
}
