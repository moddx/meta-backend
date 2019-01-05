//
// Created by Matthias Ervens on 19/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include "rabbitmqconfig.h"

#include <AMQPcpp.h>


namespace META { namespace Backend { namespace MQ {

    /**
     * Adapter class to conveniently connect to RabbitMQ.
     * Uses the AMQP-CPP library to connect to a configured
     * RabbitMQ server instance.
    */
    class RabbitMqAdapter {
    public:
        explicit RabbitMqAdapter(RabbitMqConfig config);
        RabbitMqAdapter(RabbitMqConfig config, int (*onMessage) (AMQPMessage *), int (*onCancel) (AMQPMessage *));

        explicit RabbitMqAdapter(
                std::string host = "127.0.0.1",
                int port = 5672,
                std::string exchange = "meta.production",
                std::string readQueue = "meta.production.work",
                std::string writeQueue = "meta.production.results",
                std::string readRoutingKey = "work",
                std::string writeRoutingKey = "results",
                std::string user = "guest",
                std::string password = "guest"
        );

        void setOnCancel(int (*f) (AMQPMessage *));
        void setOnMessage(int (*f) (AMQPMessage *));
        void startConsuming();  // blocking
        void stop();

        std::string inferResultRoutingKey(string work_key);

        void publish(std::string message);
        void publish(std::string message, std::string routing_key);

    private:
        void init();
        AMQP *amqp;
        AMQPExchange *exchange;
        AMQPQueue *readQueue;
        AMQPQueue *writeQueue;
        RabbitMqConfig config;
    };

}}}