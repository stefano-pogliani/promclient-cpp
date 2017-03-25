// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "promclient/counter.h"
#include "promclient/exceptions.h"
#include "promclient/metric.h"

using promclient::Counter;
using promclient::CounterDecrease;
using promclient::DescriptorRef;
using promclient::DescriptorsList;

using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;


TEST(Counter, DescribeOneCounterType) {
  DescriptorsList all_descs;
  std::set<std::string> labels;

  Counter counter("name", "comment");
  all_descs = counter.describe();
  ASSERT_EQ(static_cast<std::size_t>(1), all_descs.size());

  DescriptorRef desc = all_descs[0];
  ASSERT_EQ("name", desc->name());
  ASSERT_EQ("counter", desc->type());
  ASSERT_EQ(labels, desc->labels());
}

TEST(Counter, HashChangesOnNameAndHelp) {
  Counter counter1("name1", "comment1");
  Counter counter2("name1", "comment2");
  Counter counter3("name2", "comment1");
  Counter counter4("name2", "comment2");

  DescriptorsList descs = counter1.describe();
  std::size_t hash1 = descs[0]->hash();
  descs = counter2.describe();
  std::size_t hash2 = descs[0]->hash();
  descs = counter3.describe();
  std::size_t hash3 = descs[0]->hash();
  descs = counter4.describe();
  std::size_t hash4 = descs[0]->hash();

  ASSERT_NE(hash1, hash2);
  ASSERT_NE(hash1, hash3);
  ASSERT_NE(hash1, hash4);
  ASSERT_NE(hash2, hash3);
  ASSERT_NE(hash2, hash4);
  ASSERT_NE(hash3, hash4);
}

TEST(Counter, CollectsOneMetric) {
  Counter counter("name", "comment");
  std::map<std::string, std::string> labels;
  MetricsList metrics = counter.collect();
  ASSERT_EQ(static_cast<std::size_t>(1), metrics.size());
  ASSERT_EQ(static_cast<std::size_t>(1), metrics[0].samples().size());

  Metric metric = metrics[0];
  DescriptorRef desc = metric.descriptor();
  ASSERT_EQ("comment", desc->help());
  ASSERT_EQ("name", desc->name());
  ASSERT_EQ("counter", desc->type());

  Sample sample = metric.samples()[0];
  ASSERT_EQ("", sample.role());
  ASSERT_EQ(labels, sample.labels());
  ASSERT_EQ(0, sample.value());
}

TEST(Counter, StartsAtValue) {
  Counter counter("name", "comment", 42);
  MetricsList metrics = counter.collect();
  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ(42, sample.value());
}

TEST(Counter, DefaultIncrement) {
  Counter counter("name", "comment");
  counter.inc();
  counter.inc();
  counter.inc();
  MetricsList metrics = counter.collect();
  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ(3, sample.value());
}

TEST(Counter, IncrementByValue) {
  Counter counter("name", "comment");
  counter.inc(21);
  counter.inc(21);
  MetricsList metrics = counter.collect();
  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ(42, sample.value());
}

TEST(Counter, NegativeIncrementFails) {
  Counter counter("name", "comment");
  ASSERT_THROW(counter.inc(-1), CounterDecrease);
  MetricsList metrics = counter.collect();
  Sample sample = metrics[0].samples()[0];
  ASSERT_EQ(0, sample.value());
}
