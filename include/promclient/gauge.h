// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_GAUGE_H_
#define PROMCLIENT_GAUGE_H_

#include <atomic>
#include <memory>
#include <string>

#include "promclient/collector.h"


namespace promclient {

  //! Gauge represents a value that can go up and down.
  class Gauge : public Collector {
   public:
    Gauge(std::string name, std::string help, double initial = 0);

    MetricsList collect();
    DescriptorsList describe();

    void dec(double value = 1);
    void inc(double value = 1);
    void set(double value);

   protected:
    std::string help_;
    std::string name_;
    std::atomic<double> value_;

    DescriptorRef descriptor_;

    void update(double value);
  };
  typedef std::shared_ptr<Gauge> GaugeRef;


  //! Gauge with labels.
  class LabelledGauge : public LabelledCollector<Gauge> {
   public:
    LabelledGauge(
        std::string name, std::string help,
        std::set<std::string> labels
    );

   protected:
    std::string help_;
    std::string name_;

    virtual Ref makeChild();
  };
  typedef std::shared_ptr<LabelledGauge> LabelledGaugeRef;

}  // namespace promclient

#endif  // PROMCLIENT_GAUGE_H_
