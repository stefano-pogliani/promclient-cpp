// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/gauge.h"

using promclient::Gauge;
using promclient::LabelledGauge;

using promclient::DescriptorsList;
using promclient::MetricsList;


Gauge::Gauge(std::string name, std::string help, double initial)
  : value_(initial) {
  this->help_ = help;
  this->name_ = name;
  this->descriptor_ = DescriptorRef(new Descriptor(
      this->name_, "gauge", this->help_, {}
  ));
}


MetricsList Gauge::collect() {
  MetricsList metrics;
  Sample sample("", this->value_, {});
  metrics.push_back(Metric(this->descriptor_, {sample}));
  return metrics;
}

DescriptorsList Gauge::describe() {
  return DescriptorsList({
      this->descriptor_
  });
}


void Gauge::dec(double value) {
  this->update(-value);
}

void Gauge::inc(double value) {
  this->update(value);
}

void Gauge::set(double value) {
  this->value_.store(value);
}


void Gauge::update(double value) {
  // Perform a read-modify-write in a loop to eventually update
  // the stored value without conflicts with other treads.
  double stored = this->value_.load();
  while (!value_.compare_exchange_weak(stored, stored + value)) {
    // Noop.
  }
}


LabelledGauge::LabelledGauge(
    std::string name, std::string help,
    std::set<std::string> labels
) : LabelledCollector<Gauge>(labels) {
  this->help_ = help;
  this->name_ = name;
}

LabelledGauge::Ref LabelledGauge::makeChild() {
  return LabelledGauge::Ref(new Gauge(this->name_, this->help_));
}
