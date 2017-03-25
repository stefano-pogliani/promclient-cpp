// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_TEXT_FORMATTER_H_
#define PROMCLIENT_INTERNAL_TEXT_FORMATTER_H_

#include <string>

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

}  // namespace internal
}  // namespace promclient

#endif  // PROMCLIENT_INTERNAL_TEXT_FORMATTER_H_
