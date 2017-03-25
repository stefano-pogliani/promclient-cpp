// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_COLLECTOR_H_
#define PROMCLIENT_COLLECTOR_H_

#include <memory>
#include <mutex>
#include <vector>

#include "promclient/metric.h"

namespace promclient {

  //! Abstract metric collector.
  class Collector {
   public:
    virtual ~Collector() = default;

    //! Return zero or more collected metrics.
    virtual MetricsList collect() = 0;

    //! Returns zero or more metric descriptors.
    virtual DescriptorsList describe() = 0;
  };

  //! Skip dealing with memory allocation directly.
  typedef std::shared_ptr<Collector> CollectorRef;


  //! Template class to support labels.
  /*!
   * Collectors can either support labels directly (usually custom collectors)
   * or they can be extended with the use of labeled collectors.
   *
   * This templated class uses the decorator pattern to extend an existing
   * collector to support labels.
   *
   * Labelled collectors do not have values directly but are containers for
   * child collectors that have a fix set of label values.
   *
   * When a new set of label values is provided (with the labels method)
   * a new ChildCollector is added to the container and a refernece to it
   * is returned.
   *
   * The value of the child collector should be accessed through the
   * labeled collector only.
   */
  template<typename ChildCollector>
  class LabelledCollector : public Collector {
   public:
    //! Alias shared_prt to child collector for ease.
    typedef std::shared_ptr<ChildCollector> Ref;

   public:
    LabelledCollector(std::set<std::string> labels);

    MetricsList collect();
    DescriptorsList describe();

    //! Removes all cached collectors.
    void clear();

    //! Returns a collector with the given labels.
    Ref labels(std::map<std::string, std::string> labels);

    //! Removes a cached collector.
    void remove(std::map<std::string, std::string> labels);

   protected:
    //! Keep track of children by label values hash.
    std::map<std::size_t, Ref> children_;
    std::map<std::size_t, std::map<std::string, std::string>> labels_by_hash_;

    DescriptorsList descriptors_;
    std::set<std::string> labels_;

    std::mutex lock_labels_;

    //! Create a new instance of a child collector.
    virtual Ref makeChild() = 0;
  };

}  // namespace promclient

#include "promclient/collector.inc.h"

#endif  // PROMCLIENT_COLLECTOR_H_
