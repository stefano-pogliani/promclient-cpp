// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "promclient/gauge.h"


using promclient::Gauge;
using promclient::LabelledGauge;

using promclient::DescriptorRef;
using promclient::DescriptorsList;

using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;


class GaugeTest : public ::testing::Test {
 public:
  GaugeTest() : gauge_("name", "comment") {
    // Noop.
  }

  int collect() {
    return this->collect(&this->gauge_);
  }

  int collect(Gauge* gauge) {
    MetricsList metrics = gauge->collect();
    Sample sample = metrics[0].samples()[0];
    return sample.value();
  }

 protected:
  Gauge gauge_;
};

TEST_F(GaugeTest, DescribeOneGaugeType) {
  DescriptorsList all_descs = this->gauge_.describe();
  ASSERT_EQ(static_cast<std::size_t>(1), all_descs.size());

  std::set<std::string> labels;
  DescriptorRef desc = all_descs[0];
  ASSERT_EQ("name", desc->name());
  ASSERT_EQ("gauge", desc->type());
  ASSERT_EQ(labels, desc->labels());
}

TEST_F(GaugeTest, CollectsOneMetric) {
  std::map<std::string, std::string> labels;
  MetricsList metrics = this->gauge_.collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ(static_cast<std::size_t>(1), metrics[0].samples().size());

  Metric metric = metrics[0];
  DescriptorRef desc = metric.descriptor();
  ASSERT_EQ("comment", desc->help());
  ASSERT_EQ("name", desc->name());
  ASSERT_EQ("gauge", desc->type());

  Sample sample = metric.samples()[0];
  ASSERT_EQ("", sample.role());
  ASSERT_EQ(labels, sample.labels());
  ASSERT_EQ(0, sample.value());
}

TEST_F(GaugeTest, StartsAtValue) {
  Gauge gauge("name", "comment", 42);
  ASSERT_EQ(42, this->collect(&gauge));
}

TEST_F(GaugeTest, DecrementByDefault) {
  this->gauge_.dec();
  ASSERT_EQ(-1, this->collect());
}

TEST_F(GaugeTest, DecrementByValue) {
  this->gauge_.dec(42);
  ASSERT_EQ(-42, this->collect());
}

TEST_F(GaugeTest, IncrementByDefault) {
  this->gauge_.inc();
  ASSERT_EQ(1, this->collect());
}

TEST_F(GaugeTest, IncrementByValue) {
  this->gauge_.inc(42);
  ASSERT_EQ(42, this->collect());
}

TEST_F(GaugeTest, OverridesValue) {
  this->gauge_.inc(42);
  this->gauge_.set(33);
  ASSERT_EQ(33, this->collect());
}


class LabelledGaugeTest : public ::testing::Test {
 public:
  LabelledGaugeTest() : gauge_("name", "comment", {"lb1"}) {
    // Noop.
  }

 protected:
  LabelledGauge gauge_;
};

TEST_F(LabelledGaugeTest, Collect) {
  std::map<std::string, std::string> labels = {{"lb1", "val1"}};
  this->gauge_.labels({{"lb1", "val1"}})->set(33);

  MetricsList metrics = this->gauge_.collect();
  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ(33, sample.value());
  ASSERT_EQ(labels, sample.labels());
}
