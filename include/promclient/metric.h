// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_METRIC_H_
#define PROMCLIENT_METRIC_H_

#include <map>
#include <set>
#include <string>
#include <vector>

namespace promclient {

  //! Descriptor of a metric returned by a Collector.
  /*!
   * This information is used to validate the metrics
   * exported againat other registered metrics.
   */
  class Descriptor {
   public:
    Descriptor(
        std::string name, std::string type,
        std::set<std::string> labels
    );

    std::set<std::string> labels() const;
    std::string name() const;
    std::string type() const;

   protected:
    std::set<std::string> labels_;
    std::string name_;
    std::string type_;
  };


  //! Immutable store for a collected metric.
  class Metric {
   public:
    //! Validates a metric label name.
    /*!
     * Throws an InvalidMetricLabel std::runtime_exception
     * if the label name fails validation.
     */
    static void ValidateLabel(std::string label_name);

    //! Validates a metric name.
    /*!
     * Throws an InvalidMetricName std::runtime_exception
     * if the name fails validation.
     */
    static void ValidateName(std::string name);

    //! Validates a metric type.
    /*!
     * Throws an InvalidMetricType std::runtime_exception
     * if the type fails validation.
     */
    static void ValidateType(std::string type);

   public:
    //! Create a new metric observation.
    /*!
     * Input is NOT validated and it MUST be validated by
     * callers of this constructor.
     *
     * Metrics should create by the Collector::make_metric(...)
     */
    Metric(
        std::string name, double value,
        std::map<std::string, std::string> labels,
        std::string type = "", std::string help = ""
    );

    //! The labels attached to the metric.
    std::map<std::string, std::string> labels() const;

    //! Description of the metric (optional).
    std::string help() const;

    //! The name of the metric.
    std::string name() const;

    //! Type of the metric (optional).
    /*!
     * Known metric types are:
     *
     *   * counter
     *   * gauge
     *   * summary
     *   * histogram
     *   * untyped
     *
     * To not emit a # TYPE, set to the empty string.
     */
    std::string type() const;

    //! The observed value.
    double value() const;

   protected:
    std::map<std::string, std::string> labels_;
    std::string help_;
    std::string name_;
    std::string type_;
    double value_;
  };

}  // namespace promclient

#endif  // PROMCLIENT_METRIC_H_
