// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_INTERNAL_UTILS_H_
#define PROMCLIENT_INTERNAL_UTILS_H_

#include <functional>
#include <map>
#include <string>
#include <vector>


namespace promclient {
namespace internal {

  //! Combine the given vector of hashes into an hash.
  std::size_t CombineHashes(const std::vector<std::size_t>& hashes);

  //! Generate an hash for the given labels map.
  std::size_t HashLabels(const std::map<std::string, std::string>& labels);

}  // namespace internal
}  // namespace promclient

#endif  // PROMCLIENT_INTERNAL_UTILS_H_
