// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/exceptions.h"

using promclient::CounterDecrease;
using promclient::InvalidCollectionStrategy;
using promclient::InvalidCollector;
using promclient::InvalidMetricLabel;
using promclient::InvalidMetricName;

using promclient::HelplessCollector;
using promclient::MissingCollectorLabels;
using promclient::NamelessCollector;

using promclient::UndefinedLabel;
using promclient::UnexpectedLabel;


CounterDecrease::CounterDecrease(std::string name) :
  std::runtime_error("Attempted to decrease counter " + name)
{
  // Noop.
}

InvalidCollectionStrategy::InvalidCollectionStrategy() :
  std::runtime_error("Attempted collection with an unsupported strategy")
{
  // Noop.
}

InvalidCollector::InvalidCollector(std::string what) :
  std::runtime_error(what)
{
  // Noop.
}

InvalidMetricLabel::InvalidMetricLabel(std::string name) :
  std::runtime_error(
      "Metric label '" + name + "' is not a valid Prometheous label"
  )
{
  // Noop.
}

InvalidMetricName::InvalidMetricName(std::string name) :
  std::runtime_error(
      "Metric name '" + name + "' is not a valid Prometheous name"
  )
{
  // Noop.
}

HelplessCollector::HelplessCollector() :
  std::runtime_error("Cannot create a collector without a description")
{
  // Noop.
}

MissingCollectorLabels::MissingCollectorLabels() :
  std::runtime_error("Labelled collectors must have at least one label")
{
  // Noop.
}

NamelessCollector::NamelessCollector() :
  std::runtime_error("Cannot create a collector without a name")
{
  // Noop.
}

UndefinedLabel::UndefinedLabel(std::string label) :
  std::runtime_error("A value for label " + label + " is required")
{
  // Noop.
}

UnexpectedLabel::UnexpectedLabel(std::string label) :
  std::runtime_error("Received a value for unkown label " + label)
{
  // Noop.
}
