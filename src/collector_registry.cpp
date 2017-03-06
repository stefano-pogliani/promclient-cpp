// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/collector_registry.h"

#include <mutex>

#include "promclient/exceptions.h"

using promclient::CollectorRef;
using promclient::CollectorRegistry;
using promclient::InvalidCollector;


std::string join(std::set<std::string> values, std::string sep) {
  std::string result = "";
  std::set<std::string>::iterator it;
  for (it = values.begin(); it != values.end(); it++) {
    if (it != values.begin()) {
      result += sep;
    }
    result += *it;
  }
  return result;
}


void CollectorRegistry::Register(CollectorRef collector) {
  std::vector<Descriptor> descriptors = collector->Describe();
  std::vector<Descriptor>::iterator desc;

  // Ensure at least one metric is collected.
  if (descriptors.size() == 0) {
    throw InvalidCollector("Collector does not export any metric.");
  }

  // Lock the registry so we can check the new collector.
  std::lock_guard<std::mutex> lock(this->mutex_);

  // Copy global settings for us to check and update them.
  std::map<std::string, std::set<std::string>> metrics_labels;
  std::map<std::string, std::string> metrics_type;
  metrics_labels = this->metrics_labels_;
  metrics_type = this->metrics_type_;

  // Validate the metrics exported by the new collector.
  for (desc = descriptors.begin(); desc != descriptors.end(); desc++) {
    std::string name = desc->name();
    std::string type = desc->type();
    std::set<std::string> labels = desc->labels();
    bool have_metric = metrics_type.find(name) != metrics_type.end();

    // -> Ensure metric names have consistent types.
    if (have_metric && metrics_type[name] != type) {
      throw InvalidCollector(
          "Metric " + name + " already declared with type " +
          metrics_type[name] + " which is not compatible with type " + type
      );
    }
    metrics_type[name] = type;

    // -> Ensure metrics have consistent labels.
    if (have_metric && metrics_labels[name] != labels) {
      std::string new_labels = join(labels, ", ");
      std::string old_labels = join(metrics_labels[name], ", ");
      throw InvalidCollector(
          "Metric " + name + " already declared with labels " +
          old_labels + " which is not compatible with labels " +
          new_labels
      );
    }
    metrics_labels[name] = labels;
  }

  // Update internal state and add the collector to the registry.
  this->metrics_labels_ = metrics_labels;
  this->metrics_type_ = metrics_type;
  this->collectors_.push_back(collector);
}
