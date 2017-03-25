// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_COLLECTOR_INC_H_
#define PROMCLIENT_COLLECTOR_INC_H_

#include "promclient/exceptions.h"
#include "promclient/internal/utils.h"

namespace promclient {

  template<typename ChildCollector>
  LabelledCollector<ChildCollector>::LabelledCollector(
      std::set<std::string> labels
  ) {
    this->labels_ = labels;
  }

  template<typename ChildCollector>
  MetricsList LabelledCollector<ChildCollector>::collect() {
    std::lock_guard<std::mutex> lock(this->lock_labels_);
    MetricsList my_metrics;

    for (auto pair : this->children_) {
      std::size_t child_id = pair.first;
      std::map<std::string, std::string> child_labels;
      MetricsList child_metrics = pair.second->collect();
      child_labels = this->labels_by_hash_[child_id];

      for (auto metric : child_metrics) {
        std::vector<Sample> my_samples;
        for (auto sample : metric.samples()) {
          std::map<std::string, std::string> labels = sample.labels();
          labels.insert(child_labels.begin(), child_labels.end());
          Sample new_sample(sample.role(), sample.value(), labels);
          my_samples.push_back(new_sample);
        }
        my_metrics.push_back(Metric(metric.descriptor(), my_samples));
      }
    }
    return my_metrics;
  }

  template<typename ChildCollector>
  DescriptorsList LabelledCollector<ChildCollector>::describe() {
    std::lock_guard<std::mutex> lock(this->lock_labels_);
    if (this->descriptors_.size() == 0) {
      LabelledCollector<ChildCollector>::Ref child = this->makeChild();
      DescriptorsList descs = child->describe();

      for (auto desc : descs) {
        std::set<std::string> labels = desc->labels();
        labels.insert(this->labels_.begin(), this->labels_.end());
        this->descriptors_.push_back(DescriptorRef(new Descriptor(
            desc->name(), desc->type(), desc->help(), labels
        )));
      }
    }
    return this->descriptors_;
  }

  template<typename ChildCollector>
  void LabelledCollector<ChildCollector>::clear() {
    std::lock_guard<std::mutex> lock(this->lock_labels_);
    this->labels_by_hash_.clear();
    this->children_.clear();
  }

  template<typename ChildCollector>
  std::shared_ptr<ChildCollector> LabelledCollector<ChildCollector>::labels(
      std::map<std::string, std::string> labels
  ) {
    std::size_t hash = promclient::internal::HashLabels(labels);
    std::lock_guard<std::mutex> lock(this->lock_labels_);

    // Attempt to find cached collector.
    if (this->children_.find(hash) != this->children_.end()) {
      return this->children_.at(hash);
    }

    // This is a new labels set.
    // Check labels are all set an no unkown labels are passed.
    for (std::string label : this->labels_) {
      if (labels.find(label) == labels.end()) {
        throw promclient::UndefinedLabel(label);
      }
    }
    for (auto pair : labels) {
      if (this->labels_.find(pair.first) == this->labels_.end()) {
        throw promclient::UnexpectedLabel(pair.first);
      }
    }

    // Create a new child and cache it.
    std::shared_ptr<ChildCollector> child = this->makeChild();
    this->children_.insert(std::make_pair(hash, child));
    this->labels_by_hash_.insert(std::make_pair(hash, labels));
    return child;
  }

  template<typename ChildCollector>
  void LabelledCollector<ChildCollector>::remove(
      std::map<std::string, std::string> labels
  ) {
    std::size_t hash = promclient::internal::HashLabels(labels);
    std::lock_guard<std::mutex> lock(this->lock_labels_);
    this->labels_by_hash_.erase(this->labels_by_hash_.find(hash));
    this->children_.erase(this->children_.find(hash));
  }

}  // namespace promclient

#endif  // PROMCLIENT_COLLECTOR_INC_H_
