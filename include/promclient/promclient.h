// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef PROMCLIENT_PROMCLIENT_H_
#define PROMCLIENT_PROMCLIENT_H_

#define PROMCLIENT_VERSION "0.1.2"

// This is just an include wrapper to make all needed headers
// available to library users.
#include "promclient/counter.h"
#include "promclient/gauge.h"

#include "promclient/internal/builder_counter.h"
#include "promclient/internal/builder_gauge.h"

#endif  // PROMCLIENT_PROMCLIENT_H_
