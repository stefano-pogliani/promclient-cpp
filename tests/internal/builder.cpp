// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

#include "promclient/collector.h"
#include "promclient/collector_registry.h"
#include "promclient/counter.h"
#include "promclient/exceptions.h"
#include "promclient/internal/builder.h"


using promclient::CollectorRef;
using promclient::CollectorRegistry;
using promclient::DescriptorRef;
using promclient::DescriptorsList;
using promclient::MetricsList;
using promclient::Sample;

using promclient::Counter;
using promclient::CounterRef;
using promclient::LabelledCounter;
using promclient::LabelledCounterRef;

using promclient::InvalidCollector;
using promclient::InvalidMetricLabel;
using promclient::InvalidMetricName;

using promclient::HelplessCollector;
using promclient::MissingCollectorLabels;
using promclient::NamelessCollector;

using promclient::internal::SimpleBuilder;
using promclient::internal::SimpleLabelledBuilder;


class TestRegistry : public CollectorRegistry {
 public:
  CollectorRef getCollector(int idx) {
    return this->collectors_[idx];
  }
};


class TestLabelledBuilder : public SimpleLabelledBuilder<LabelledCounter> {
 public:
  std::set<std::string> getLabels() const {
    return this->labels_;
  }

  std::string getHelp() const {
    return this->help_;
  }

  std::string getName() const {
    return this->name_;
  }
};


TEST(SimpleLabelledBuilder, LabelsAreSet) {
  std::set<std::string> expected_labels = {"lb2", "lb1"};
  TestLabelledBuilder builder;
  builder.labels({"lb1", "lb2"});
  ASSERT_EQ(expected_labels, builder.getLabels());
}

TEST(SimpleLabelledBuilder, LabelsMustBeSet) {
  TestLabelledBuilder builder;
  builder.name("test_name");
  builder.help("used for testing");
  ASSERT_THROW(builder.labels({}), MissingCollectorLabels);
  ASSERT_THROW(builder.build(), MissingCollectorLabels);
}

TEST(SimpleLabelledBuilder, LabelsMustBeValid) {
  TestLabelledBuilder builder;
  builder.name("test_name");
  builder.help("used for testing");
  ASSERT_THROW(builder.labels({"lb0", ""}), InvalidMetricLabel);
  ASSERT_THROW(builder.labels({"__"}), InvalidMetricLabel);
}


TEST(SimpleLabelledBuilder, HelpIsSet) {
  TestLabelledBuilder builder;
  builder.help("used for testing");
  ASSERT_EQ("used for testing", builder.getHelp());
}

TEST(SimpleLabelledBuilder, HelpMustBeSet) {
  TestLabelledBuilder builder;
  builder.name("test_name");
  ASSERT_THROW(builder.build(), HelplessCollector);
}

TEST(SimpleLabelledBuilder, HelpMustBeSetButCanBeEmpty) {
  TestLabelledBuilder builder;
  builder.name("test_name");
  builder.help("");
  builder.labels({"lb0"});
  ASSERT_NO_THROW(builder.build());
}


TEST(SimpleLabelledBuilder, NameIsSet) {
  TestLabelledBuilder builder;
  builder.name("test_counter");
  ASSERT_EQ("test_counter", builder.getName());
}

TEST(SimpleLabelledBuilder, NameMustBeSet) {
  TestLabelledBuilder builder;
  ASSERT_THROW(builder.build(), NamelessCollector);
}

TEST(SimpleLabelledBuilder, NameMustBeValid) {
  TestLabelledBuilder builder;
  ASSERT_THROW(builder.name(""), InvalidMetricName);
}


TEST(SimpleLabelledBuilder, Build) {
  std::set<std::string> expected_labels = {"lb2", "lb1"};
  std::map<std::string, std::string> expected_values = {
    {"lb2", "val2"},
    {"lb1", "val1"}
  };

  SimpleLabelledBuilder<LabelledCounter> builder;
  LabelledCounterRef counters = builder.name("test_name").help(
      "used for testing"
  ).labels({"lb1", "lb2"}).build();
  ASSERT_NE(nullptr, counters.get());

  DescriptorsList descs = counters->describe();
  ASSERT_EQ(static_cast<std::size_t>(1), descs.size());

  DescriptorRef desc = descs[0];
  ASSERT_EQ("test_name", desc->name());
  ASSERT_EQ("used for testing", desc->help());
  ASSERT_EQ(expected_labels, desc->labels());

  CounterRef counter = counters->labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  counter->inc(55);

  MetricsList metrics = counters->collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ(static_cast<std::size_t>(1), metrics[0].samples().size());

  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ("", sample.role());
  ASSERT_EQ(55, sample.value());
  ASSERT_EQ(expected_values, sample.labels());
}

TEST(SimpleLabelledBuilder, Register) {
  TestRegistry registry;
  SimpleLabelledBuilder<LabelledCounter> builder;
  LabelledCounterRef counter = builder.name("test_name").help(
      "used for testing"
  ).labels({"lb1", "lb2"}).registr(&registry);
  ASSERT_EQ(counter, registry.getCollector(0));
}

TEST(SimpleLabelledBuilder, RegisterDefault) {
  SimpleLabelledBuilder<LabelledCounter> builder;
  builder.name("test_name").help("").labels({"lb1", "lb2"}).registr();
  ASSERT_THROW(
      builder.name("test_name").help("").labels({"lb1"}).registr(),
      InvalidCollector
  );
}


class TestBuilder : public SimpleBuilder<Counter, LabelledCounter> {
 public:
  std::string getHelp() const {
    return this->help_;
  }

  std::string getName() const {
    return this->name_;
  }
};


TEST(SimpleBuilder, LabelsAreOptional) {
  TestBuilder builder;
  builder.name("test_metric");
  builder.help("test");
  ASSERT_NO_THROW(builder.build());
}

TEST(SimpleBuilder, LabelsCarriesHelp) {
  TestBuilder builder;
  SimpleLabelledBuilder<LabelledCounter> labelled_builder = \
      builder.name("test_name").help("test").labels({"lb1", "lb2"});
  ASSERT_NO_THROW(labelled_builder.build());
}

TEST(SimpleBuilder, LabelsCarriesName) {
  TestBuilder builder;
  SimpleLabelledBuilder<LabelledCounter> labelled_builder = \
      builder.name("test_name").labels({"lb1", "lb2"});
  ASSERT_THROW(labelled_builder.build(), HelplessCollector);
}

TEST(SimpleBuilder, LabelsMustBeValid) {
  TestBuilder builder;
  ASSERT_THROW(builder.labels({"lb0", ""}), InvalidMetricLabel);
  ASSERT_THROW(builder.labels({"__"}), InvalidMetricLabel);
}


TEST(SimpleBuilder, HelpIsSet) {
  TestBuilder builder;
  builder.help("used for testing");
  ASSERT_EQ("used for testing", builder.getHelp());
}

TEST(SimpleBuilder, HelpMustBeSet) {
  TestBuilder builder;
  builder.name("test_name");
  ASSERT_THROW(builder.build(), HelplessCollector);
}

TEST(SimpleBuilder, HelpMustBeSetButCanBeEmpty) {
  TestBuilder builder;
  builder.name("test_name");
  builder.help("");
  ASSERT_NO_THROW(builder.build());
}


TEST(SimpleBuilder, NameIsSet) {
  TestBuilder builder;
  builder.name("test_counter");
  ASSERT_EQ("test_counter", builder.getName());
}

TEST(SimpleBuilder, NameMustBeSet) {
  TestBuilder builder;
  ASSERT_THROW(builder.build(), NamelessCollector);
}

TEST(SimpleBuilder, NameMustBeValid) {
  TestBuilder builder;
  ASSERT_THROW(builder.name(""), InvalidMetricName);
}


TEST(SimpleBuilder, Build) {
  std::set<std::string> expected_labels = {};
  std::map<std::string, std::string> expected_values = {};

  SimpleBuilder<Counter, LabelledCounter> builder;
  CounterRef counter = builder.name("test_name").help(
      "used for testing"
  ).build();
  ASSERT_NE(nullptr, counter.get());

  counter->inc(55);
  DescriptorsList descs = counter->describe();
  ASSERT_EQ(static_cast<std::size_t>(1), descs.size());

  DescriptorRef desc = descs[0];
  ASSERT_EQ("test_name", desc->name());
  ASSERT_EQ("used for testing", desc->help());
  ASSERT_EQ(expected_labels, desc->labels());

  MetricsList metrics = counter->collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ(static_cast<std::size_t>(1), metrics[0].samples().size());

  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ("", sample.role());
  ASSERT_EQ(55, sample.value());
  ASSERT_EQ(expected_values, sample.labels());
}

TEST(SimpleBuilder, Register) {
  TestRegistry registry;
  SimpleBuilder<Counter, LabelledCounter> builder;
  CounterRef counter = builder.name("test_name").help(
      "used for testing"
  ).registr(&registry);
  ASSERT_EQ(counter, registry.getCollector(0));
}

TEST(SimpleBuilder, RegisterDefault) {
  SimpleBuilder<Counter, LabelledCounter> builder;
  builder.name("test_name_no_labels").help("").registr();
  ASSERT_THROW(
      builder.name("test_name_no_labels").help("for tests").registr(),
      InvalidCollector
  );
}
