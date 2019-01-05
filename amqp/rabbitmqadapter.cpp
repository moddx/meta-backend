//
// Created by Matthias Ervens on 19/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include <utility>

#include <utility>

#include <utility>


#include "rabbitmqadapter.h"
#include "json.hpp"

#include <iostream>

using namespace std;
using namespace nlohmann;

namespace META { namespace Backend { namespace MQ {

    RabbitMqAdapter::RabbitMqAdapter(RabbitMqConfig config) {
        this->config = move(config);
        init();
    }

    RabbitMqAdapter::RabbitMqAdapter(RabbitMqConfig config, int (*onMessage) (AMQPMessage *), int (*onCancel) (AMQPMessage *)) {
        this->config = move(config);
        init();
        setOnMessage(onMessage);
        setOnCancel(onCancel);
    }

    RabbitMqAdapter::RabbitMqAdapter(string host, int port, string exchange,
                                     string readQueue, string writeQueue,
                                     string readRoutingKey, string writeRoutingQueue,
                                     string user, string password) {
        this->config.host = move(host);
        this->config.port = static_cast<u_int16_t>(port);
        this->config.exchange = move(exchange);
        this->config.readQueue = move(readQueue);
        this->config.writeQueue = move(writeQueue);
        this->config.readRoutingKey = move(readRoutingKey);
        this->config.writeRoutingKey = move(writeRoutingQueue);
        this->config.user = move(user);
        this->config.password = move(password);

        init();
    }


    /*
     * Note:
     * amqp-cpp uses one connection internally. Each Exchange, Queue, ..
     * operates on a separate channel.
     */
    void RabbitMqAdapter::init() {
        cout << "Connecting to RabbitMQ..\n";
        string connectionString = config.getConnectionString();
        amqp = new AMQP(connectionString);

        cout << "Creating ReadQueue..\n";
        readQueue = amqp->createQueue(config.readQueue);
        readQueue->Declare();

        cout << "Creating WriteQueue..\n";
        writeQueue = amqp->createQueue(config.writeQueue);
        writeQueue->Declare();

        cout << "Creating Exchange..\n";
        exchange = amqp->createExchange(config.exchange);
        exchange->Declare(config.exchange, "topic");

        cout << "Binding Queues to Exchange..\n";
        readQueue->Bind(config.exchange, config.readRoutingKey);
        writeQueue->Bind(config.exchange, config.writeRoutingKey);
    }

    void RabbitMqAdapter::startConsuming() {
        try {
            cout << "Starting to consume messages.\n";
            readQueue->Consume(AMQP_NOACK);
        } catch (AMQPException &e) {
            cout << "EXCEPTION: " << e.getMessage() << endl;
        }
    }

    void RabbitMqAdapter::setOnMessage(int (*f)(AMQPMessage *)) {
        if(readQueue == nullptr) {
            cerr << "ERROR! Can't use setOnMessage() when Queue is not yet initialized!\n";
            return;
        }

        readQueue->addEvent(AMQP_MESSAGE, f);
    }

    void RabbitMqAdapter::setOnCancel(int (*f)(AMQPMessage *)) {
        if(readQueue == nullptr) {
            cerr << "ERROR! Can't use setOnCancel() when Queue is not yet initialized!\n";
            return;
        }

        readQueue->addEvent(AMQP_CANCEL, f);
    }

    void RabbitMqAdapter::publish(string message, string routing_key) {
        exchange->setHeader("Delivery-mode", 2);
        exchange->setHeader("Content-type", "application/json");
        exchange->setHeader("Content-encoding", "UTF-8");

        cout << "Publishing with routing key: " << routing_key << "\n";

        exchange->Publish(move(message), move(routing_key));
    }

    void RabbitMqAdapter::publish(string message) {
        publish(move(message), config.writeRoutingKey);
    }

    std::string RabbitMqAdapter::inferResultRoutingKey(string work_key) {
        string clientid = work_key.substr(work_key.find_last_of('.'));
        return config.writeRoutingKey + clientid;
    }

    void RabbitMqAdapter::stop() {
        readQueue->closeChannel();
        amqp->closeChannel();
    }
}

}}