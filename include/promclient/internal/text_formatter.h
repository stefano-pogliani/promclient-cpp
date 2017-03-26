// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_TEXT_FORMATTER_H_
#define PROMCLIENT_INTERNAL_TEXT_FORMATTER_H_

#include <string>

#include "promclient/collector_registry.h"
#include "promclient/metric.h"


namespace promclient {
namespace internal {

  //! Helper class for generating Prometheous text format.
  /*!
   * See https://prometheus.io/docs/instrumenting/exposition_formats/#text-format-details
   */
  class TextFormatter {
   public:
    //! Format HELP and TYPE lines for a descriptor.
    std::string describe(DescriptorRef descriptor);

    //! Format a metric sample.
    std::string sample(std::string name, Sample sample);
  };


  //! Abstract class to share TextFormatter code.
  class TextFormatBridge {
   public:
    explicit TextFormatBridge(CollectorRegistry* registry);

    //! Collect metrics form the register and calls write for each metric.
    void collect();

   protected:
    TextFormatter formatter;
    CollectorRegistry* registry_;
    CollectorRegistry::CollectStrategy strategy_;

    //! Writes a formatted line.
    virtual void write(std::string line) = 0;
  };

}  // namespace internal
}  // namespace promclient

#endif  // PROMCLIENT_INTERNAL_TEXT_FORMATTER_H_
