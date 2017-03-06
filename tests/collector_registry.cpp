// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <vector>

#include "promclient/collector.h"
#include "promclient/collector_registry.h"
#include "promclient/exceptions.h"
#include "promclient/metric.h"

using promclient::Collector;
using promclient::CollectorRef;
using promclient::CollectorRegistry;

using promclient::Descriptor;
using promclient::InvalidCollector;
using promclient::Metric;


class MockCollector : public Collector {
 public:
  MockCollector() = default;
  MockCollector(std::vector<Descriptor> descriptors) {
    this->descriptors = descriptors;
  }

  std::vector<Descriptor> Describe() {
    return this->descriptors;
  }

  std::vector<Metric> Collect() {
    return std::vector<Metric>();
  }

 protected:
  std::vector<Descriptor> descriptors;
};


class TestRegistry : public CollectorRegistry {
 public:
  int CountCollectors() {
    return this->collectors_.size();
  }
};


class CollectorRegistryTest : public ::testing::Test {
 public:
  TestRegistry registry;
};


class RegisterTest : public CollectorRegistryTest {};


TEST_F(RegisterTest, AddCollector) {
  CollectorRef collector(new MockCollector({
      Descriptor{"test_name", "counter", {}}
  }));
  this->registry.Register(collector);
  ASSERT_EQ(1, this->registry.CountCollectors());
}

TEST_F(RegisterTest, MustCollectAtLeastOneMetric) {
  CollectorRef collector = std::make_shared<MockCollector>();
  ASSERT_THROW(this->registry.Register(collector), InvalidCollector);
}

TEST_F(RegisterTest, MetricsWithSameNameHaveSameType) {
  CollectorRef collector0(new MockCollector({
      Descriptor{"test_name", "counter", {}},
      Descriptor{"test_name", "gauge", {}}
  }));
  ASSERT_THROW(this->registry.Register(collector0), InvalidCollector);

  CollectorRef collector1(new MockCollector({
      Descriptor{"test_name", "counter", {}}
  }));
  CollectorRef collector2(new MockCollector({
      Descriptor{"test_name", "gauge", {}}
  }));
  this->registry.Register(collector1);
  ASSERT_THROW(this->registry.Register(collector2), InvalidCollector);

  CollectorRef collector3(new MockCollector({
      Descriptor{"test_name", "counter", {}},
      Descriptor{"test_name", "counter", {}}
  }));
  CollectorRef collector4(new MockCollector({
      Descriptor{"test_name", "counter", {}}
  }));
  this->registry.Register(collector3);
  this->registry.Register(collector4);
}

TEST_F(RegisterTest, MetricsWithSameNameHaveSameLabels) {
  CollectorRef collector0(new MockCollector({
      Descriptor{"test_name", "counter", {"lbl1", "lbl2"}},
      Descriptor{"test_name", "counter", {"lbl1"}}
  }));
  ASSERT_THROW(this->registry.Register(collector0), InvalidCollector);

  CollectorRef collector1(new MockCollector({
      Descriptor{"test_name", "counter", {"lbl1", "lbl2"}}
  }));
  CollectorRef collector2(new MockCollector({
      Descriptor{"test_name", "counter", {"lbl2"}}
  }));
  this->registry.Register(collector1);
  ASSERT_THROW(this->registry.Register(collector2), InvalidCollector);

  CollectorRef collector3(new MockCollector({
      Descriptor{"test_name", "counter", {"lbl1", "lbl2"}}
  }));
  CollectorRef collector4(new MockCollector({
      Descriptor{"test_name", "counter", {"lbl2", "lbl1"}}
  }));
  this->registry.Register(collector3);
  this->registry.Register(collector4);
}
