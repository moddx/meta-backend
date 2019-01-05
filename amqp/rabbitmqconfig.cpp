//
// Created by Matthias Ervens on 19/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include "rabbitmqconfig.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <json.hpp>

using namespace nlohmann;

namespace META { namespace Backend { namespace MQ {

    string RabbitMqConfig::getConnectionString()  {
        stringstream s;
        s << user << ":" << password << "@" << host << ":" << port;
        return s.str();
    }

    RabbitMqConfig RabbitMqConfig::of(const string &path)  {
        cout << "Reading MQ config from file: " << path << " ..\n";
        ifstream ifs (path);

        // is file readable..
        if(!ifs.good()) {
            stringstream s;
            s << "Can't open file at " << path << ". Either it does not exist or we don't have read access.\n";
            throw runtime_error(s.str());
        }

        // read file into json object
        cout << "Reading and parsing in progress.. ";
        json j;
        ifs >> j;
        ifs.close();
        cout << "Done.\n";

        // expect a json object as root
        if(!j.is_object()) {
            stringstream s;
            s << "Can't parse JSON! Expected type Object but is " << j.type_name() << "\n";
            throw runtime_error(s.str());
        }

        cout << j.dump() << endl;

        cout << "Extracting individual fields.. ";

        cout.flush();

        // extract data into config
        RabbitMqConfig config;
        config.host = j["host"].get<string>();
        config.port = j["port"].get<int>(); cout << ".";
        config.exchange = j["exchange"].get<string>(); cout << ".";
        config.readQueue = j["read_queue"].get<string>(); cout << ".";
        config.readRoutingKey = j["read_routing_key"].get<string>(); cout << ".";
        config.writeQueue = j["write_queue"].get<string>(); cout << ".";
        config.writeRoutingKey = j["write_routing_key"].get<string>(); cout << ".";

        cout << "Done.\n";

        return config;
    }

}}}