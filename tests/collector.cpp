// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <vector>

#include "promclient/collector.h"
#include "promclient/exceptions.h"

using promclient::Collector;
using promclient::Descriptor;
using promclient::DescriptorRef;
using promclient::DescriptorsList;

using promclient::LabelledCollector;
using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;

using promclient::UndefinedLabel;
using promclient::UnexpectedLabel;


class ConstCollector : public Collector {
 public:
  ConstCollector() {
    this->descriptor_ = DescriptorRef(new Descriptor(
        "test", "gauge", "comment", {"lb0"}
    ));
  }

  MetricsList collect() {
    Sample sample("", 3, {{"lb0", "val0"}});
    return MetricsList({
        Metric(this->descriptor_, {sample})
    });
  }

  DescriptorsList describe() {
    return DescriptorsList({this->descriptor_});
  }

 protected:
  DescriptorRef descriptor_;
};


class TestCollector : public LabelledCollector<ConstCollector> {
 public:
  TestCollector(std::set<std::string> labels) :
    LabelledCollector<ConstCollector>(labels) {
    // Noop.
  }

 protected:
  std::shared_ptr<ConstCollector> makeChild() {
    return std::shared_ptr<ConstCollector>(new ConstCollector());
  }
};


TEST(LabelledCollector, DescribeDecoratesChildCollector) {
  TestCollector test({"lb1", "lb2"});
  DescriptorsList descs = test.describe();
  std::set<std::string> labels = {"lb0", "lb1", "lb2"};
  ASSERT_EQ(static_cast<std::size_t>(1), descs.size());

  DescriptorRef desc = descs[0];
  ASSERT_EQ("test", desc->name());
  ASSERT_EQ("gauge", desc->type());
  ASSERT_EQ("comment", desc->help());
  ASSERT_EQ(labels, desc->labels());
}

TEST(LabelledCollector, DescribeIsCached) {
  TestCollector test({"lb1", "lb2"});
  DescriptorsList descs1 = test.describe();
  DescriptorsList descs2 = test.describe();
  ASSERT_EQ(descs1[0], descs2[0]);
}

TEST(LabelledCollector, CollectLabeledMetrics) {
  TestCollector test({"lb1", "lb2"});
  TestCollector::Ref collector1 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });

  std::map<std::string, std::string> expected_labels = {
    {"lb0", "val0"},
    {"lb1", "val1"},
    {"lb2", "val2"}
  };

  MetricsList metrics = test.collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());

  std::vector<Sample> samples = metrics[0].samples();
  ASSERT_EQ(static_cast<std::size_t>(1), samples.size());
  ASSERT_EQ("", samples[0].role());
  ASSERT_EQ(3, samples[0].value());
  ASSERT_EQ(expected_labels, samples[0].labels());
}

TEST(LabelledCollector, CollectNothingWithoutLabels) {
  TestCollector test({"lb1", "lb2"});
  MetricsList metrics = test.collect();
  ASSERT_EQ(static_cast<std::size_t>(0), metrics.size());
}

TEST(LabelledCollector, LablesReturnsACollector) {
  TestCollector test({"lb1", "lb2"});
  TestCollector::Ref collector = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  ASSERT_NE(nullptr, collector.get());
}

TEST(LabelledCollector, LabelsReturnsTheSameCollector) {
  TestCollector test({"lb1", "lb2"});
  TestCollector::Ref collector1 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  TestCollector::Ref collector2 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  TestCollector::Ref collector3 = test.labels({
      {"lb2", "val2"},
      {"lb1", "val1"}
  });
  ASSERT_EQ(collector1, collector2);
  ASSERT_EQ(collector1, collector3);
}

TEST(LabelledCollector, LabelsThrowsIfLabelsAreMissing) {
  TestCollector test({"lb1", "lb2"});
  ASSERT_THROW(test.labels({}), UndefinedLabel);
  ASSERT_THROW(test.labels({{"lb1", "val1"}}), UndefinedLabel);
}

TEST(LabelledCollector, LabelsThrowsIfExtraLabelsArePassed) {
  TestCollector test({"lb1", "lb2"});
  ASSERT_THROW(test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"},
      {"lb3", "val3"}
  }), UnexpectedLabel);
}

TEST(LabelledCollector, RemovesChild) {
  TestCollector test({"lb1", "lb2"});
  TestCollector::Ref collector1 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  test.remove({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });

  TestCollector::Ref collector2 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  ASSERT_NE(collector1, collector2);
}

TEST(LabelledCollector, ClearChildren) {
  TestCollector test({"lb1", "lb2"});
  TestCollector::Ref collector1 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  TestCollector::Ref collector2 = test.labels({
      {"lb1", "val11"},
      {"lb2", "val22"}
  });
  test.clear();

  TestCollector::Ref collector3 = test.labels({
      {"lb1", "val1"},
      {"lb2", "val2"}
  });
  TestCollector::Ref collector4 = test.labels({
      {"lb1", "val11"},
      {"lb2", "val22"}
  });
  ASSERT_NE(collector1, collector3);
  ASSERT_NE(collector2, collector4);
}
