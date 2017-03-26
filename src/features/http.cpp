// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "promclient/features/http.h"

#include <onion/onion.h>
#include <stdexcept>

#include "promclient/collector_registry.h"
#include "promclient/internal/text_formatter.h"
#include "promclient/metric.h"


using promclient::CollectorRegistry;
using promclient::MetricsList;
using promclient::Sample;

using promclient::features::HttpExporter;
using promclient::internal::TextFormatBridge;


//! Write lines to an onion response.
class OnionTextBridge : public TextFormatBridge {
 public:
  OnionTextBridge(
      CollectorRegistry* registry, onion_response* response
  ) : TextFormatBridge(registry) {
    this->response_ = response;
  }

  void setContentType() {
    onion_response_set_header(
        this->response_, "Content-Type",
        "text/plain; version=0.0.4"
    );
  }

 protected:
  onion_response* response_;

  void write(std::string line) {
    onion_response_write0(this->response_, line.c_str());
  }
};


HttpExporter::HttpExporter(
    CollectorRegistry* registry,
    std::string host, std::string port
) {
  this->host_  = host;
  this->onion_ = nullptr;
  this->port_  = port;

  this->registry_ = registry;
  if (this->registry_ == nullptr) {
    this->registry_ = CollectorRegistry::Default();
  }
}

HttpExporter::~HttpExporter() {
  if (this->onion_) {
    onion_free(this->onion_);
  }
}


std::string HttpExporter::endpoint() {
  if (!this->onion_) {
    throw std::runtime_error("Need to call mount first");
  }
  return "http://" + this->host_ + ":" + this->port_;
}

void HttpExporter::listen() {
  if (!this->onion_) {
    throw std::runtime_error("Need to call mount first");
  }
  onion_listen(this->onion_);
}

void HttpExporter::mount() {
  // Create the onion.
  this->onion_ = onion_new(O_THREADED);
  onion_set_hostname(this->onion_, this->host_.c_str());
  onion_set_port(this->onion_, this->port_.c_str());

  // Add paths
  onion_url* urls = onion_root_url(this->onion_);
  onion_url_add_static(urls, "", "See /metrics", HTTP_OK);
  onion_url_add_handler(
      urls, "metrics", onion_handler_new(
        HttpExporter::CallMetrics, this, nullptr
      )
  );
}

void HttpExporter::stop() {
  if (!this->onion_) {
    throw std::runtime_error("Need to call mount first");
  }
  onion_listen_stop(this->onion_);
}


onion_connection_status HttpExporter::CallMetrics(
    void* instance, onion_request* request,
    onion_response* response
) {
  if (!instance) {
    throw std::runtime_error("Unable to call exporter back");
  }
  HttpExporter* server = static_cast<HttpExporter*>(instance);
  return server->metrics(request, response);
}


onion_connection_status HttpExporter::metrics(
    onion_request* request, onion_response* response
) {
  OnionTextBridge bridge(this->registry_, response);
  bridge.setContentType();
  bridge.collect();
  return OCS_PROCESSED;
}
