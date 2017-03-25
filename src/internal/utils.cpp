// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/internal/utils.h"

#include <functional>
#include <map>
#include <string>
#include <vector>


// From boost implementation of hash_combine.
// https://github.com/boostorg/functional/blob/boost-1.63.0/include/boost/functional/hash/hash.hpp#L210
std::size_t promclient::internal::CombineHashes(
    const std::vector<std::size_t>& hashes
) {
  std::size_t combined = 0;
  for (auto hash : hashes) {
    combined ^= hash + 0x9e3779b9 + (combined << 6) + (combined >> 2);
  }
  return combined;
}

std::size_t promclient::internal::HashLabels(
    const std::map<std::string, std::string>& labels
) {
  std::vector<std::size_t> hashes;
  for (auto pair : labels) {
    hashes.push_back(std::hash<std::string>()(pair.first));
    hashes.push_back(std::hash<std::string>()(pair.second));
  }
  return promclient::internal::CombineHashes(hashes);
}
