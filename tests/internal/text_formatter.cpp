// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "promclient/metric.h"

#include "promclient/internal/text_formatter.h"


using promclient::Descriptor;
using promclient::DescriptorRef;
using promclient::Sample;

using promclient::internal::TextFormatter;


class TextFormatterTest : public ::testing::Test {
 public:
  DescriptorRef descriptor(
      std::string name, std::string help,
      std::string type
  ) {
    return DescriptorRef(new Descriptor(name, type, help, {}));
  }

 protected:
  TextFormatter formatter;
};


TEST_F(TextFormatterTest, DescribeHelpEscapesNewLine) {
  std::string expected = "";
  expected += "# HELP metric for\\n testing\n";
  expected += "# TYPE metric counter\n";

  DescriptorRef desc = this->descriptor("metric", "for\n testing", "counter");
  std::string description = this->formatter.describe(desc);
  ASSERT_EQ(expected, description);
}

TEST_F(TextFormatterTest, DescribeHelpEscapesSlash) {
  std::string expected = "";
  expected += "# HELP metric for\\\\ testing\n";
  expected += "# TYPE metric counter\n";

  DescriptorRef desc = this->descriptor("metric", "for\\ testing", "counter");
  std::string description = this->formatter.describe(desc);
  ASSERT_EQ(expected, description);
}

TEST_F(TextFormatterTest, DescribeHelpAndType) {
  std::string expected = "";
  expected += "# HELP metric for testing\n";
  expected += "# TYPE metric counter\n";

  DescriptorRef desc = this->descriptor("metric", "for testing", "counter");
  std::string description = this->formatter.describe(desc);
  ASSERT_EQ(expected, description);
}

TEST_F(TextFormatterTest, DescribeTypeNoHelp) {
  std::string expected = "";
  expected += "# TYPE metric counter\n";

  DescriptorRef desc = this->descriptor("metric", "", "counter");
  std::string description = this->formatter.describe(desc);
  ASSERT_EQ(expected, description);
}


TEST_F(TextFormatterTest, WriteSampleWithRoleAndNoLabels) {
  Sample sample("and_role", 5, {});
  std::string expected = "metric_name_and_role 5.0000000000000000e+00\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteSampleWithoutRoleOrLabels) {
  Sample sample("", 5, {});
  std::string expected = "metric_name 5.0000000000000000e+00\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteSampleWithoutRoleWithLabels) {
  Sample sample("", 5, {{"lb2", "val2"}, {"lb1", "val1"}});
  std::string expected;
  expected += "metric_name{lb1=\"val1\",lb2=\"val2\"}";
  expected += " 5.0000000000000000e+00\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteSampleLabelValueEscapesNewLine) {
  Sample sample("", 5, {{"l", "v\n1"}});
  std::string expected = "metric_name{l=\"v\\n1\"} 5.0000000000000000e+00\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteSampleLabelValueEscapesQuote) {
  Sample sample("", 5, {{"l", "v\"1"}});
  std::string expected = "metric_name{l=\"v\\\"1\"} 5.0000000000000000e+00\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteSampleLabelValueEscapesSlash) {
  Sample sample("", 5, {{"l", "v\\1"}});
  std::string expected = "metric_name{l=\"v\\\\1\"} 5.0000000000000000e+00\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}


TEST_F(TextFormatterTest, WriteValuePositiveInf) {
  double inf = std::numeric_limits<double>::infinity();
  Sample sample("", inf, {});
  std::string expected = "metric_name +Inf\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteValueNegativeInf) {
  double inf = std::numeric_limits<double>::infinity();
  Sample sample("", -inf, {});
  std::string expected = "metric_name -Inf\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteValueNan) {
  // Use a zero variable to avoid "devide by zero" warning.
  double zero = 0;
  double nan = 0 / zero;
  Sample sample("", nan, {});
  std::string expected = "metric_name NaN\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}

TEST_F(TextFormatterTest, WriteValueDecimal) {
  Sample sample("", 185592.73536698326e+5, {});
  std::string expected = "metric_name 1.8559273536698326e+10\n";
  std::string line = this->formatter.sample("metric_name", sample);
  ASSERT_EQ(expected, line);
}
