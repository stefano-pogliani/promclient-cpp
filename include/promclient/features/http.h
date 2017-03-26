// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_FEATURES_HTTP_H_
#define PROMCLIENT_FEATURES_HTTP_H_

#include <onion/onion.h>
#include <string>

#include "promclient/collector_registry.h"


namespace promclient {
namespace features {

  //! HTTP server (based on libonion) to run expose metrics.
  class HttpExporter {
   protected:
    static onion_connection_status CallMetrics(
        void* instance, onion_request* request,
        onion_response* response
    );

   public:
    explicit HttpExporter(
        CollectorRegistry* registry = nullptr,
        std::string host = "127.0.0.1", std::string port = "9200"
    );
    ~HttpExporter();

    //! Returns the endpoint we are listening on.
    std::string endpoint();

    //! Start the onion server.
    void listen();

    //! Create the onion and mount the enpoints.
    void mount();

    //! Stops the server.
    void stop();

   protected:
    std::string host_;
    std::string port_;

    onion* onion_;
    CollectorRegistry* registry_;

    //! Handles requests to /metrics
    onion_connection_status metrics(
        onion_request* request, onion_response* response
    );
  };

}  // namespace features
}  // namespace promclient

#endif  // PROMCLIENT_FEATURES_HTTP_H_
