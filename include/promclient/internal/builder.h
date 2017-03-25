// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_BUILDER_H_
#define PROMCLIENT_INTERNAL_BUILDER_H_

#include <memory>
#include <set>
#include <string>

#include "promclient/collector_registry.h"

namespace promclient {
namespace internal {

  //! Templated builder for simple metric with labels.
  /*!
   * Collectors built by SimpleLabelledBuilder must have
   * a constructor with the following signature:
   *
   *  Collector(
   *    std::string name, std::string help,
   *    std::set<std::string> labels
   *  )
   */
  template<typename Collector>
  class SimpleLabelledBuilder {
   public:
    SimpleLabelledBuilder();
    SimpleLabelledBuilder(const SimpleLabelledBuilder<Collector>&) = default;

    //! Set the allowed metric labels.
    SimpleLabelledBuilder<Collector> labels(std::set<std::string> labels);

    //! Set the metric description.
    SimpleLabelledBuilder<Collector> help(std::string help);

    //! Set the metric name.
    SimpleLabelledBuilder<Collector> name(std::string name);

    //! Returns a new Collector.
    std::shared_ptr<Collector> build();

    //! Register and return a new Collector.
    /*!
     * The missing `e` in `registr` is to avoid clashes
     * with the C keyword `register`.
     */
    std::shared_ptr<Collector> registr(CollectorRegistry* registry = nullptr);

   protected:
    bool help_set_;
    std::string help_;
    std::string name_;
    std::set<std::string> labels_;
  };


  //! Templated builder for simple metric without labels.
  /*!
   * Collectors built by SimpleBuilder must have
   * a constructor with the following signature:
   *
   *  Collector(std::string name, std::string help)
   */
  template<typename Collector, typename Labelled>
  class SimpleBuilder {
   public:
    SimpleBuilder();
    SimpleBuilder(const SimpleBuilder<Collector, Labelled>&) = default;

    //! Set the allowed metric labels.
    SimpleLabelledBuilder<Labelled> labels(std::set<std::string> labels);

    //! Set the metric description.
    SimpleBuilder<Collector, Labelled> help(std::string help);

    //! Set the metric name.
    SimpleBuilder<Collector, Labelled> name(std::string name);

    //! Returns a new Collector.
    std::shared_ptr<Collector> build();

    //! Register and return a new Collector.
    /*!
     * The missing `e` in `registr` is to avoid clashes
     * with the C keyword `register`.
     */
    std::shared_ptr<Collector> registr(CollectorRegistry* registry = nullptr);

   protected:
    bool help_set_;
    std::string help_;
    std::string name_;
  };

}  // namespace internal
}  // namespace promclient

#include "promclient/internal/builder.inc.h"

#endif  // PROMCLIENT_INTERNAL_BUILDER_H_
