//
// Created by Matthias Ervens on 12/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include "distributor.h"
#include "amqp/rabbitmqadapter.h"
#include <jsonhelper.h>

#include <chrono>
#include <iostream>
#include <csignal>

using namespace std;
using namespace std::chrono_literals;
using namespace META::Core;
using namespace META::Backend;
using namespace META::Backend::MQ;


class MyComputeUnit : public ComputeUnit {                // inject computeunit classname here..
public:
    ComputeData* compute(ComputeData *data) const override;
};


Distributor *distributor;
RabbitMqAdapter *amqp;

u_int64_t msgCount = 0;

int onCancel(AMQPMessage *message) {
    cout << "cancel tag=" << message->getDeliveryTag() << endl;
    return 0;
}

void publishResults(const string &apikey, const string &routingkey, const vector<DataSet> &results) {
    string reply = JsonHelper::toJson(results, apikey);

    cout << "Sending reply..\n";
    amqp->publish(reply, routingkey);
}

int  onMessage(AMQPMessage *message) {
    msgCount++;
    uint32_t i = 0;
    char *data = message->getMessage(&i);

    cout << "------------------\n";
    cout << "Received message #" << msgCount << " tag=" << message->getDeliveryTag() << " content-type:" << message->getHeader("Content-type") ;
    cout << " encoding:" << message->getHeader("Content-encoding") << " mode="<<message->getHeader("Delivery-mode") << endl;
    if (data) cout << data << endl;


    string apikey;
    vector<DataSet> sets;
    cout << "Parsing data as json..";
    try {
        JsonHelper::fromJson(data, sets, apikey);
    } catch(json::exception &e) {
        cerr << "\nException parsing/extracting json data! " << e.what();
        return 1;
    }
    cout << "Done. Extracted " << sets.size() << " datasets.\n";

    string publishkey = amqp->inferResultRoutingKey(message->getRoutingKey());

    cout << "Distributing the work..\n";
    distributor->distribute(Request(move(apikey), move(publishkey), DataSource(sets)));

    cout << "------------------\n";
    return 0;
};

void shutdownHandling(int signal) {
    cout << "Received Signal " << signal << ". Shutting down gracefully..\n";
    if(distributor != nullptr)
        distributor->shutdown();
    if(amqp != nullptr)
        amqp->stop();
    exit(0);
}

int main(int argc, char **argv) {
    signal(SIGINT, shutdownHandling);
    signal(SIGTERM, shutdownHandling);
    cout << "Starting META::Backend instance for: MyComputeUnit" << endl;   // ..and inject here..

    ComputeUnit *cu = new MyComputeUnit();                                  // ..and here.
    distributor = new Distributor(cu, 2, publishResults);

    amqp = new RabbitMqAdapter(RabbitMqConfig::of("rabbitMQ_conf.json"), onMessage, onCancel);
    amqp->startConsuming();

    cout << "Shutting down.." << endl;
    distributor->shutdown();
    amqp->stop();

    return 0;
}
