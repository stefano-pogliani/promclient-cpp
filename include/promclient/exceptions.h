// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_EXCEPTIONS_H_
#define PROMCLIENT_EXCEPTIONS_H_

#include <stdexcept>
#include <string>

namespace promclient {

  //! Thrown when adding a colletor to a register clashes.
  class InvalidCollector : public std::runtime_error {
   public:
    explicit InvalidCollector(std::string what);
  };

}  // namespace promclient

#endif  // PROMCLIENT_EXCEPTIONS_H_
