PromClient
==========
A lightwaight, dependency free, Prometheous client for C++11.


Features
--------
Prometheous already has a C++ client so why write another one?

  * Standard C++11 code.
  * No required dependencies for core features.
  * Features that require dependencies are disabled by default.
  * Built with `make` to be usable in all projects.

Aside for the differnece with other libraries, PromClient features include:

  * Follows the official Prometheous client structure.
  * Supports Prometheous Text Format (0.4.0).
  * Features that require dependencies (libraries or OS) are
    optionally enabled at compile time (see below).


Optional features
-----------------
Some of the feature that PromClient supports are not enabled
by defautl and are option.
This has several advantages but the fundamental ones are:

  * Can use the core library on a platform that does not
    support extentions.
  * Keeps the core library small and simple.

The following is a list of optional features provided and how
to enable them.

### HTTP exposer
An HTTP server to expose a `/metrics` endpoint is provided
as optional to avoid the extra dependency on LibOnion.

  1. The code is provided by a git submodule in `features/onion`.
  2. Add `FEAT_HTTP=1` to make commands.
  3. When linking the final binary add:
    * The `out/libonion_static.a` static library.
    * The `pthread` dynamic library.


Usage
-----
PromClient is a static library that should be integrated in
other applications/libraries to provide insight to users and
administrators of the final library.

Add metrics to you code, typically as file level variables:
```c++
#include <promclient/promclien.h>

using promclient::CounterRef;
using promclient::LabelledCounterRef;

CounterRef total_requests = promclient::CounterBuilder()
  .name("total_requests")
  .help("Total number of requests served by the application")
  .registr();  // Note the missing `e` (because register is a keyword).

LabelledCounterRef handled_events = promclient::CounterBuilder()
  .name("handled_events")
  .help("Number of events handled by type")
  .labels({"event", "user"})
  .registr();


// Manipulate the metrics in your methods.
void handle_request_type_a(std::string user, ...) {
  total_requests->inc();
  handled_events->labels({
    {"event", "a"},
    {"user", user}
  })->inc();
}

// Your program needs to export the metrics.
// An HttpExporter is optionally provided to run an HTTP server
// that exports the metrics at /metrics
```

Compile the library and link it with your program:
```bash
make clean  # optional
make build

g++ --std=c++11 -I<promclient/root/dir/inclide> -o your_app \
  ... promclient/root/dir/out/libpromclient.a
```
