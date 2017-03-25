// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_BUILDER_INC_H_
#define PROMCLIENT_INTERNAL_BUILDER_INC_H_

#include "promclient/collector_registry.h"
#include "promclient/metric.h"

namespace promclient {
namespace internal {

  template<typename Collector>
  SimpleLabelledBuilder<Collector>::SimpleLabelledBuilder() {
    this->help_set_ = false;
  }

  template<typename Collector>
  SimpleLabelledBuilder<Collector>
  SimpleLabelledBuilder<Collector>::labels(std::set<std::string> labels) {
    if (labels.size() == 0) {
      throw MissingCollectorLabels();
    }
    for (std::string label : labels) {
      Metric::ValidateLabel(label);
    }
    this->labels_ = labels;
    return *this;
  }

  template<typename Collector>
  SimpleLabelledBuilder<Collector>
  SimpleLabelledBuilder<Collector>::help(std::string help) {
    this->help_ = help;
    this->help_set_ = true;
    return *this;
  }

  template<typename Collector>
  SimpleLabelledBuilder<Collector>
  SimpleLabelledBuilder<Collector>::name(std::string name) {
    Metric::ValidateName(name);
    this->name_ = name;
    return *this;
  }

  template<typename Collector>
  std::shared_ptr<Collector> SimpleLabelledBuilder<Collector>::build() {
    // Check name is set.
    if (this->name_ == "") {
      throw NamelessCollector();
    }

    // Check help is set, empty help is ok.
    if (!this->help_set_) {
      throw HelplessCollector();
    }

    // Check labels are set (no use for a labelled collector without labels).
    if (this->labels_.size() == 0) {
      throw MissingCollectorLabels();
    }

    return std::shared_ptr<Collector>(new Collector(
          this->name_, this->help_, this->labels_
    ));
  }

  template<typename Collector>
  std::shared_ptr<Collector>
  SimpleLabelledBuilder<Collector>::registr(CollectorRegistry* registry) {
    if (!registry) {
      registry = &CollectorRegistry::Default();
    }
    std::shared_ptr<Collector> collector = this->build();
    registry->registr(collector);
    return collector;
  }


  template<typename Collector, typename Labelled>
  SimpleBuilder<Collector, Labelled>::SimpleBuilder() {
    this->help_set_ = false;
  }

  template<typename Collector, typename Labelled>
  SimpleLabelledBuilder<Labelled>
  SimpleBuilder<Collector, Labelled>::labels(std::set<std::string> labels) {
    SimpleLabelledBuilder<Labelled> builder;
    if (this->name_ != "") {
      builder.name(this->name_);
    }
    if (this->help_set_) {
      builder.help(this->help_);
    }
    return builder.labels(labels);
  }

  template<typename Collector, typename Labelled>
  SimpleBuilder<Collector, Labelled>
  SimpleBuilder<Collector, Labelled>::help(std::string help) {
    this->help_ = help;
    this->help_set_ = true;
    return *this;
  }

  template<typename Collector, typename Labelled>
  SimpleBuilder<Collector, Labelled>
  SimpleBuilder<Collector, Labelled>::name(std::string name) {
    Metric::ValidateName(name);
    this->name_ = name;
    return *this;
  }

  template<typename Collector, typename Labelled>
  std::shared_ptr<Collector>
  SimpleBuilder<Collector, Labelled>::build() {
    // Check name is set.
    if (this->name_ == "") {
      throw NamelessCollector();
    }

    // Check help is set, empty help is ok.
    if (!this->help_set_) {
      throw HelplessCollector();
    }

    return std::shared_ptr<Collector>(new Collector(this->name_, this->help_));
  }

  template<typename Collector, typename Labelled>
  std::shared_ptr<Collector>
  SimpleBuilder<Collector, Labelled>::registr(CollectorRegistry* registry) {
    if (!registry) {
      registry = &CollectorRegistry::Default();
    }
    std::shared_ptr<Collector> collector = this->build();
    registry->registr(collector);
    return collector;
  }

}  // namespace internal
}  // namespace promclient

#endif  // PROMCLIENT_INTERNAL_BUILDER_INC_H_
