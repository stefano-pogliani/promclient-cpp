// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_COUNTER_H_
#define PROMCLIENT_COUNTER_H_

#include <memory>
#include <mutex>
#include <string>

#include "promclient/collector.h"


namespace promclient {

  //! Simple ever-increasing counter.
  class Counter : public Collector {
   public:
    Counter(std::string name, std::string help, double initial = 0);

    //! Increment the counter by value (1 by default).
    void inc(double value = 1);

    virtual MetricsList collect();
    virtual DescriptorsList describe();

   protected:
    std::string help_;
    std::string name_;
    double value_;

    //! Descriptor of the counter.
    DescriptorRef descriptor_;

    //! Thread safe access to the value.
    std::mutex mutex_;
  };
  typedef std::shared_ptr<Counter> CounterRef;


  //! Simple ever-increasing counter with labels.
  class LabelledCounter : public LabelledCollector<Counter> {
   public:
    LabelledCounter(
        std::string name, std::string help,
        std::set<std::string> labels
    );

   protected:
    std::string help_;
    std::string name_;

    virtual Ref makeChild();
  };
  typedef std::shared_ptr<LabelledCounter> LabelledCounterRef;

}  // namespace promclient

#endif  // PROMCLIENT_COUNTER_H_
