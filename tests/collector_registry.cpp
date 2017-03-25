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
using promclient::DescriptorRef;
using promclient::DescriptorsList;

using promclient::InvalidCollectionStrategy;
using promclient::InvalidCollector;

using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;


class MockCollector : public Collector {
 public:
  MockCollector() = default;
  MockCollector(DescriptorsList descriptors) {
    this->descriptors_ = descriptors;
  }

  MetricsList collect() {
    return this->metrics_;
  }

  DescriptorsList describe() {
    return this->descriptors_;
  }

 protected:
  MetricsList metrics_;
  DescriptorsList descriptors_;
};


class FixedCollector : public MockCollector {
 public:
  explicit FixedCollector(std::string name, std::vector<Sample> samples) {
    this->descriptors_.push_back(DescriptorRef(
        new Descriptor(name, "untyped", "", {})
    ));
    this->metrics_.push_back(Metric(this->descriptors_[0], samples));
  }
};


class TestRegistry : public CollectorRegistry {
 public:
  int countCollectors() {
    return this->collectors_.size();
  }
};


class CollectorRegistryTest : public ::testing::Test {
 public:
  TestRegistry registry;

  DescriptorRef descriptor(std::string type, std::set<std::string> labels) {
    return DescriptorRef(new Descriptor("test_name", type, "comment", labels));
  }
};


class RegisterTest : public CollectorRegistryTest {};

TEST_F(RegisterTest, AddCollector) {
  CollectorRef collector(new MockCollector({
      this->descriptor("counter", {})
  }));
  this->registry.registr(collector);
  ASSERT_EQ(1, this->registry.countCollectors());
}

TEST_F(RegisterTest, MustCollectAtLeastOneMetric) {
  CollectorRef collector = std::make_shared<MockCollector>();
  ASSERT_THROW(this->registry.registr(collector), InvalidCollector);
}

TEST_F(RegisterTest, MetricsWithSameNameHaveSameType) {
  CollectorRef collector0(new MockCollector({
      this->descriptor("counter", {}),
      this->descriptor("gauge", {})
  }));
  ASSERT_THROW(this->registry.registr(collector0), InvalidCollector);

  CollectorRef collector1(new MockCollector({
      this->descriptor("counter", {})
  }));
  CollectorRef collector2(new MockCollector({
      this->descriptor("gauge", {})
  }));
  this->registry.registr(collector1);
  ASSERT_THROW(this->registry.registr(collector2), InvalidCollector);

  CollectorRef collector3(new MockCollector({
      this->descriptor("counter", {}),
      this->descriptor("counter", {})
  }));
  CollectorRef collector4(new MockCollector({
      this->descriptor("counter", {})
  }));
  this->registry.registr(collector3);
  this->registry.registr(collector4);
}

TEST_F(RegisterTest, MetricsWithSameNameHaveSameLabels) {
  CollectorRef collector0(new MockCollector({
      this->descriptor("counter", {"lbl1", "lbl2"}),
      this->descriptor("counter", {"lbl1"})
  }));
  ASSERT_THROW(this->registry.registr(collector0), InvalidCollector);

  CollectorRef collector1(new MockCollector({
      this->descriptor("counter", {"lbl1", "lbl2"})
  }));
  CollectorRef collector2(new MockCollector({
      this->descriptor("counter", {"lbl2"})
  }));
  this->registry.registr(collector1);
  ASSERT_THROW(this->registry.registr(collector2), InvalidCollector);

  CollectorRef collector3(new MockCollector({
      this->descriptor("counter", {"lbl1", "lbl2"})
  }));
  CollectorRef collector4(new MockCollector({
      this->descriptor("counter", {"lbl2", "lbl1"})
  }));
  this->registry.registr(collector3);
  this->registry.registr(collector4);
}

TEST_F(RegisterTest, RemoveCollector) {
  CollectorRef collector(new MockCollector({
      this->descriptor("counter", {})
  }));
  this->registry.registr(collector);
  this->registry.registr(collector);
  ASSERT_EQ(2, this->registry.countCollectors());

  ASSERT_TRUE(this->registry.unregister(collector));
  ASSERT_EQ(0, this->registry.countCollectors());
}


class CollectTest : public CollectorRegistryTest {
 public:
  void addCollector(std::string name) {
    this->addCollector(name, {
        Sample("", 3, {})
    });
  }

  void addCollector(std::string name, std::vector<Sample> samples) {
    CollectorRef collector(new FixedCollector(name, samples));
    this->registry.registr(collector);
  }
};

TEST_F(CollectTest, InvalidStrategyThrows) {
  CollectorRegistry::CollectStrategy invalid_strategy = \
    static_cast<CollectorRegistry::CollectStrategy>(-33);
  ASSERT_THROW(
      this->registry.collect(invalid_strategy),
      InvalidCollectionStrategy
  );
}


class SortedCollectTest : public CollectTest {
 public:
  MetricsList collect() {
    return this->registry.collect(CollectorRegistry::CollectStrategy::SORTED);
  }
};

TEST_F(SortedCollectTest, SortsMetricsByName) {
  this->addCollector("def");
  this->addCollector("abc");
  MetricsList metrics = this->collect();
  ASSERT_EQ(static_cast<std::size_t>(2), metrics.size());
  ASSERT_EQ("abc", metrics[0].descriptor()->name());
  ASSERT_EQ("def", metrics[1].descriptor()->name());
}

TEST_F(SortedCollectTest, SortsMetricsByRole) {
  this->addCollector("abc", {
      Sample("role2", 1, {}),
      Sample("role1", 2, {})
  });
  MetricsList metrics = this->collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ("abc", metrics[0].descriptor()->name());

  std::vector<Sample> samples = metrics[0].samples();
  ASSERT_EQ(static_cast<std::size_t>(2), samples.size());
  ASSERT_EQ("role1", samples[0].role());
  ASSERT_EQ("role2", samples[1].role());
}

TEST_F(SortedCollectTest, SortsMetricsByRoleDifferentCollectors) {
  this->addCollector("abc", {Sample("role2", 1, {})});
  this->addCollector("abc", {Sample("role1", 2, {})});
  MetricsList metrics = this->collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ("abc", metrics[0].descriptor()->name());

  std::vector<Sample> samples = metrics[0].samples();
  ASSERT_EQ(static_cast<std::size_t>(2), samples.size());
  ASSERT_EQ("role1", samples[0].role());
  ASSERT_EQ("role2", samples[1].role());
}

TEST_F(SortedCollectTest, SortsMetricsByLabels) {
  this->addCollector("abc", {
      Sample("role1", 2, {{"lb1", "val2"}}),
      Sample("role1", 1, {{"lb1", "val1"}})
  });
  MetricsList metrics = this->collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ("abc", metrics[0].descriptor()->name());

  std::vector<Sample> samples = metrics[0].samples();
  ASSERT_EQ(static_cast<std::size_t>(2), samples.size());
  ASSERT_EQ(1, samples[0].value());
  ASSERT_EQ(2, samples[1].value());
}

TEST_F(SortedCollectTest, SortsMetricsByLabelsDifferentCollectors) {
  this->addCollector("abc", {
      Sample("role1", 2, {{"lb1", "val2"}})
  });
  this->addCollector("abc", {
      Sample("role1", 1, {{"lb1", "val1"}})
  });
  MetricsList metrics = this->collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ("abc", metrics[0].descriptor()->name());

  std::vector<Sample> samples = metrics[0].samples();
  ASSERT_EQ(static_cast<std::size_t>(2), samples.size());
  ASSERT_EQ(1, samples[0].value());
  ASSERT_EQ(2, samples[1].value());
}
