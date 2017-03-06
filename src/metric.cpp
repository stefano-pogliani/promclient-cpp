// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/metric.h"

#include <map>
#include <set>
#include <string>
#include <vector>

using promclient::Descriptor;
using promclient::Metric;


Descriptor::Descriptor(
    std::string name, std::string type,
    std::set<std::string> labels
) {
  this->labels_ = labels;
  this->name_ = name;
  this->type_ = type;
}

std::set<std::string> Descriptor::labels() const {
  return this->labels_;
}

std::string Descriptor::name() const {
  return this->name_;
}

std::string Descriptor::type() const {
  return this->type_;
}


void Metric::ValidateLabel(std::string label_name) {
  // TODO(stefano): validate label name.
}

void Metric::ValidateName(std::string name) {
  // TODO(stefano): validate name.
}

void Metric::ValidateType(std::string type) {
  // TODO(stefano): validate type.
}


Metric::Metric(
    std::string name, double value,
    std::map<std::string, std::string> labels,
    std::string type, std::string help
) {
  this->labels_ = labels;
  this->help_ = help;
  this->name_ = name;
  this->type_ = type;
  this->value_ = value;
}

std::map<std::string, std::string> Metric::labels() const {
  return this->labels_;
}

std::string Metric::help() const {
  return this->help_;
}

std::string Metric::name() const {
  return this->name_;
}

std::string Metric::type() const {
  return this->type_;
}

double Metric::value() const {
  return this->value_;
}
