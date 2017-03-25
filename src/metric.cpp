// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/metric.h"

#include <functional>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "promclient/exceptions.h"
#include "promclient/internal/utils.h"

using promclient::Descriptor;
using promclient::DescriptorRef;

using promclient::InvalidMetricLabel;
using promclient::InvalidMetricName;

using promclient::Metric;
using promclient::Sample;

using promclient::internal::CombineHashes;


// Regular expressions to perform validation.
std::regex METRIC_LABEL_RE("[a-zA-Z_][a-zA-Z0-9_]*");
std::regex METRIC_NAME_RE("[a-zA-Z_:][a-zA-Z0-9_:]*");


Descriptor::Descriptor(
    std::string name, std::string type,
    std::string help, std::set<std::string> labels
) {
  this->labels_ = labels;
  this->help_ = help;
  this->name_ = name;
  this->type_ = type;

  // Build descriptor hash once.
  std::vector<std::size_t> hashes;
  hashes.push_back(std::hash<std::string>()(this->name_));
  hashes.push_back(std::hash<std::string>()(this->type_));
  hashes.push_back(std::hash<std::string>()(this->help_));

  for (auto label : this->labels_) {
    std::size_t chunk = std::hash<std::string>()(label);
    hashes.push_back(chunk);
  }
  this->hash_ = CombineHashes(hashes);
}

std::set<std::string> Descriptor::labels() const {
  return this->labels_;
}

std::string Descriptor::help() const {
  return this->help_;
};

std::string Descriptor::name() const {
  return this->name_;
}

std::string Descriptor::type() const {
  return this->type_;
}

std::size_t Descriptor::hash() const {
  return this->hash_;
}


void Metric::ValidateLabel(std::string label_name) {
  // Metrics are validated to match the specification at:
  // https://prometheus.io/docs/concepts/data_model/#metric-names-and-labels
  if (
      label_name.length() > 1 && label_name[0] == '_' &&
      label_name[0] == label_name[1]
  ) {
    throw InvalidMetricLabel(label_name);
  }
  if (!std::regex_match(label_name, METRIC_LABEL_RE)) {
    throw InvalidMetricLabel(label_name);
  }
}

void Metric::ValidateName(std::string name) {
  // Metrics are validated to match the specification at:
  // https://prometheus.io/docs/concepts/data_model/#metric-names-and-labels
  if (!std::regex_match(name, METRIC_NAME_RE)) {
    throw InvalidMetricName(name);
  }
}

void Metric::ValidateType(std::string type) {
  // TODO(stefano): validate type.
}


Metric::Metric(DescriptorRef descriptor, std::vector<Sample> samples) {
  this->descriptor_ = descriptor;
  this->samples_ = samples;
}

DescriptorRef Metric::descriptor() const {
  return this->descriptor_;
}

std::vector<Sample> Metric::samples() const {
  return this->samples_;
}


bool Sample::Compare::operator()(const Sample& lhs, const Sample& rhs) {
  // Complare roles first.
  if (lhs.role() < rhs.role()) {
    return true;
  }

  // Compare labels next.
  // We assume that samples have the same labels.
  std::map<std::string, std::string> rhs_labels = rhs.labels();
  for (auto pair : lhs.labels()) {
    if (pair.second < rhs_labels[pair.first]) {
      return true;
    }
  }

  // Seems like !(lhs < rhs).
  return false;
}


Sample::Sample(
    std::string role, double value,
    std::map<std::string, std::string> labels
) {
  this->labels_ = labels;
  this->role_ = role;
  this->value_ = value;
}

std::map<std::string, std::string> Sample::labels() const {
  return this->labels_;
}

std::string Sample::role() const {
  return this->role_;
}

double Sample::value() const {
  return this->value_;
}
