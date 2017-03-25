// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_EXCEPTIONS_H_
#define PROMCLIENT_EXCEPTIONS_H_

#include <stdexcept>
#include <string>

namespace promclient {

  //! Thrown when a counter is decreased.
  class CounterDecrease : public std::runtime_error {
   public:
    explicit CounterDecrease(std::string name);
  };

  //! Thrown when a registry attempts collection with an uknown strategy.
  class InvalidCollectionStrategy : public std::runtime_error {
   public:
    InvalidCollectionStrategy();
  };

  //! Thrown when adding a colletor to a register clashes.
  class InvalidCollector : public std::runtime_error {
   public:
    explicit InvalidCollector(std::string what);
  };

  //! Thrown when a metric label fails to validate.
  class InvalidMetricLabel : public std::runtime_error {
   public:
    explicit InvalidMetricLabel(std::string name);
  };

  //! Thrown when a metric name fails to validate.
  class InvalidMetricName : public std::runtime_error {
   public:
    explicit InvalidMetricName(std::string name);
  };

  //! Thrown when a builder tries to build a collector without a description.
  class HelplessCollector : public std::runtime_error {
   public:
    HelplessCollector();
  };

  //! Thrown when a labelled builder tries to build a collector without labels.
  class MissingCollectorLabels : public std::runtime_error {
   public:
    MissingCollectorLabels();
  };

  //! Thrown when a builder tries to build a collector without a name.
  class NamelessCollector : public std::runtime_error {
   public:
    NamelessCollector();
  };

  //! Thrown when a labelled collector is requested but without all labels.
  class UndefinedLabel : public std::runtime_error {
   public:
    explicit UndefinedLabel(std::string label);
  };

  //! Thrown when a labelled collector is requested with extra labels.
  class UnexpectedLabel : public std::runtime_error {
   public:
    explicit UnexpectedLabel(std::string label);
  };

}  // namespace promclient

#endif  // PROMCLIENT_EXCEPTIONS_H_
