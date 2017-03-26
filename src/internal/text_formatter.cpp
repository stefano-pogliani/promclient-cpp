// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/internal/text_formatter.h"

#include <iomanip>
#include <regex>
#include <sstream>

#include "promclient/collector_registry.h"
#include "promclient/metric.h"


using promclient::CollectorRegistry;
using promclient::DescriptorRef;
using promclient::Sample;

using promclient::internal::TextFormatBridge;
using promclient::internal::TextFormatter;


std::regex NEW_LINE_RE = std::regex("\n");
std::regex QUOTE_RE = std::regex("\"");
std::regex SLASH_RE = std::regex("\\\\");


TextFormatBridge::TextFormatBridge(CollectorRegistry* registry) {
  this->registry_ = registry;
  this->strategy_ = CollectorRegistry::CollectStrategy::SORTED;
}

void TextFormatBridge::collect() {
  MetricsList metrics = this->registry_->collect(this->strategy_);
  for (auto metric : metrics) {
    DescriptorRef descriptor = metric.descriptor();
    std::string desc = formatter.describe(descriptor);
    std::string name = descriptor->name();
    this->write(desc);

    for (Sample sample : metric.samples()) {
      std::string line = formatter.sample(name, sample);
      this->write(line);
    }
  }
}


std::string TextFormatter::describe(DescriptorRef descriptor) {
  std::string help = descriptor->help();
  std::string name = descriptor->name();
  std::string type = descriptor->type();

  std::stringstream desc;
  if (help != "") {
    std::string help_ = help;
    help_ = std::regex_replace(help_, SLASH_RE, "\\\\");
    help_ = std::regex_replace(help_, NEW_LINE_RE, "\\n");
    desc << "# HELP " << name << " " << help_ << '\n';
  }

  desc << "# TYPE " << name << " " << type << '\n';
  return desc.str();
}

std::string TextFormatter::sample(std::string name, Sample sample) {
  // Start the line with the metric name.
  std::stringstream line;
  line << name;

  // Add the sample role, if any.
  if (sample.role() != "") {
    line << '_' << sample.role();
  }
  
  // Add labels, if any.
  std::map<std::string, std::string> labels = sample.labels();
  if (labels.size() != 0) {
    std::size_t count = labels.size();
    std::size_t index = 0;
    line << '{';

    for (auto pair : labels) {
      std::string name = pair.first;
      std::string value = pair.second;

      // Escape label value.
      value = std::regex_replace(value, SLASH_RE, "\\\\");
      value = std::regex_replace(value, NEW_LINE_RE, "\\n");
      value = std::regex_replace(value, QUOTE_RE, "\\\"");

      line << name << "=\"" << value << '"';
      index += 1;
      if (index != count) {
        line << ',';
      }
    }

    line << '}';
  }

  // Format value.
  double value = sample.value();
  if (isinf(value) && value > 0) {
    line << " +Inf\n";
  } else if(isinf(value) && value < 0) {
    line << " -Inf\n";
  } else if(isnan(value)) {
    line << " NaN\n";
  } else {
    auto digits = std::numeric_limits<double>::digits10 + 1;
    line << ' '
      << std::setprecision(digits)
      << std::scientific
      << value << '\n';
  }

  return line.str();
}
