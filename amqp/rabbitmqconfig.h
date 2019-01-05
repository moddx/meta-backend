//
// Created by Matthias Ervens on 19/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#ifndef META_BACKEND_RABBITMQCONFIG_H
#define META_BACKEND_RABBITMQCONFIG_H

#include <string>

using namespace std;

namespace META { namespace Backend { namespace MQ {

/**
 * A wrapper class to conveniently pass RabbitMQ
 * connection configurations around.
 */
class RabbitMqConfig {
public:
    string host = "127.0.0.1";
    int port = 5672;
    string exchange = "meta.production";
    string readQueue = "meta.production.work";
    string writeQueue = "meta.production.results";
    string readRoutingKey = "work";
    string writeRoutingKey = "results";
    string user = "guest";
    string password = "guest";

    string getConnectionString();
    static RabbitMqConfig of(const string &path);
};

static RabbitMqConfig SharedConfig = RabbitMqConfig();


}}}


#endif //META_BACKEND_RABBITMQCONFIG_H
