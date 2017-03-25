// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_METRIC_H_
#define PROMCLIENT_METRIC_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace promclient {

  //! Descriptor of a metric returned by a Collector.
  /*!
   * This information is used to validate the metrics
   * exported against other registered metrics.
   */
  class Descriptor {
   public:
    Descriptor(
        std::string name, std::string type,
        std::string help, std::set<std::string> labels
    );

    std::set<std::string> labels() const;
    std::string help() const;
    std::string name() const;

    //! Type of the metric.
    /*!
     * Known metric types are:
     *
     *   * counter
     *   * gauge
     *   * summary
     *   * histogram
     *   * untyped
     */
    std::string type() const;

    //! Return a hash for the descriptor.
    /*!
     * The hash is computed by concatenating the metric ...
     *
     *   * ... name
     *   * ... type
     *   * ... help
     *   * ... labels
     */
    std::size_t hash() const;

   protected:
    std::set<std::string> labels_;
    std::size_t hash_;
    std::string help_;
    std::string name_;
    std::string type_;
  };

  //! Reuse descriptors without having to rebuild them.
  typedef std::shared_ptr<Descriptor> DescriptorRef;
  typedef std::vector<DescriptorRef>  DescriptorsList;


  //! A single data point that is part of a metric.
  /*!
   * This is to allow both simple metrics (such as counters
   * that only have one value) as well as more complext types
   * (such as histograms) that have multiple values for each
   * metric.
   */
  class Sample {
   public:
    //! Compare operation to use in sorder std structures.
    class Compare {
     public:
      bool operator()(const Sample& lhs, const Sample& rhs);
    };

   public:
    Sample(
        std::string role, double value,
        std::map<std::string, std::string> labels
    );

    std::map<std::string, std::string> labels() const;
    std::string role() const;
    double value() const;

   protected:
    std::map<std::string, std::string> labels_;
    std::string role_;
    double value_;
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
    Metric(DescriptorRef descriptor, std::vector<Sample> samples);
    virtual ~Metric() = default;

    DescriptorRef descriptor() const;
    std::vector<Sample> samples() const;

   protected:
    DescriptorRef descriptor_;
    std::vector<Sample> samples_;
  };

  // Make code cleaner.
  typedef std::vector<Metric> MetricsList;

}  // namespace promclient

#endif  // PROMCLIENT_METRIC_H_
