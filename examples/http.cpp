// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <iostream>

#include "promclient/promclient.h"
#include "promclient/features/http.h"


using promclient::LabelledCounterRef;
using promclient::features::HttpExporter;


LabelledCounterRef example_counter = promclient::CounterBuilder()
  .name("example_counter")
  .help("An example counter that means nothing")
  .labels({"example"})
  .registr();


int main(int argc, char **argv) {
  HttpExporter server;
  server.mount();

  example_counter->labels({{"example", "http"}})->inc(22);

  std::cout << "Server listening at " << server.endpoint() << std::endl;
  server.listen();
  return 0; 
}
