// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/collector_registry.h"

#include <mutex>

#include "promclient/exceptions.h"

using promclient::CollectorRef;
using promclient::CollectorRegistry;
using promclient::DescriptorsList;

using promclient::InvalidCollectionStrategy;
using promclient::InvalidCollector;

using promclient::DescriptorRef;
using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;


CollectorRegistry default_registry_;
bool default_registry_inited_ = false;


CollectorRegistry& CollectorRegistry::Default() {
  if (!default_registry_inited_) {
    default_registry_inited_ = true;
    // TODO(stefano): register default collectors.
  }
  return default_registry_;
}


MetricsList CollectorRegistry::collect(
    CollectorRegistry::CollectStrategy strategy
) {
  switch (strategy) {
    case CollectorRegistry::CollectStrategy::SORTED:
      return this->sortedCollect();

    default:
      throw InvalidCollectionStrategy();
  }
}

void CollectorRegistry::registr(CollectorRef collector) {
  // Ensure at least one metric is collected.
  DescriptorsList descriptors = collector->describe();
  if (descriptors.size() == 0) {
    throw InvalidCollector("Collector does not export any metric.");
  }

  // Lock the registry so we can check the new collector and
  // copy global settings for us to check and update them.
  std::lock_guard<std::mutex> lock(this->mutex_);
  std::map<std::string, std::size_t> metrics_hash = this->metrics_hash_;

  // Ensure metrics are not exposed with conflicting descriptors.
  for (auto desc : descriptors) {
    std::string name = desc->name();
    std::size_t hash = desc->hash();
    bool have_metric = metrics_hash.find(name) != metrics_hash.end();

    if (have_metric && metrics_hash[name] != hash) {
      throw InvalidCollector(
          "Metric " + name + " already declared with a confliction descriptor"
      );
    }
    metrics_hash[name] = hash;
  }

  // Update internal state and add the collector to the registry.
  this->metrics_hash_ = metrics_hash;
  this->collectors_.push_back(collector);
}

bool CollectorRegistry::unregister(CollectorRef collector) {
  // Lock the registry so we can remove the collector.
  std::lock_guard<std::mutex> lock(this->mutex_);
  std::vector<CollectorRef>::iterator it;

  // Remove the collector but keep the descriptors.
  // If a collector for the metric name is added again,
  // it is good that we can ensure the metric is the same.
  std::vector<std::vector<CollectorRef>::iterator> to_remove;
  for (it = this->collectors_.begin(); it != this->collectors_.end(); it++) {
    if (it->get() == collector.get()) {
      to_remove.push_back(it);
    }
  }
  for (auto it : to_remove) {
    this->collectors_.erase(it);
  }
  return to_remove.size() != 0;
}


/*** SORTED STRATEGY ***/
struct SortedMetricRecord {
  DescriptorRef descriptor;
  std::set<Sample, Sample::Compare> samples;
};


MetricsList CollectorRegistry::sortedCollect() {
  std::vector<CollectorRef> collectors;
  std::map<std::string, SortedMetricRecord> metrics_by_name;

  // Scoped access to the collectors list to copy it so we can
  // unlock the regisrty while collection is performed.
  {
    std::lock_guard<std::mutex> lock(this->mutex_);
    collectors = this->collectors_;
  }

  // Collect all metrics and index by name.
  // The map will sort metrics by name of us.
  for (CollectorRef collector : collectors) {
    MetricsList metrics = collector->collect();
    for (Metric metric : metrics) {
      DescriptorRef desc = metric.descriptor();
      std::string name = desc->name();

      // Index the metric.
      if (metrics_by_name.find(name) == metrics_by_name.end()) {
        SortedMetricRecord record;
        record.descriptor = desc;
        metrics_by_name[name] = record;
      }

      // Add the samples.
      for (Sample sample : metric.samples()) {
        metrics_by_name[name].samples.insert(sample);
      }
    }
  }

  // Iterate over the "indexed" metrics and return them.
  MetricsList metrics;
  for (auto pair : metrics_by_name) {
    std::vector<Sample> samples(
        pair.second.samples.begin(),
        pair.second.samples.end()
    );
    Metric metric(pair.second.descriptor, samples);
    metrics.push_back(metric);
  }
  return metrics;
}
