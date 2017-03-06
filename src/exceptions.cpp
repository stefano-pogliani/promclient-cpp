// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/exceptions.h"

using promclient::InvalidCollector;


InvalidCollector::InvalidCollector(std::string what) :
  std::runtime_error(what)
{
  // Noop.
}
