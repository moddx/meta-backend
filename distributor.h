//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include <string>
#include <map>

#include "computeunit.h"
#include "datasource.h"
#include "threadpool/blockingqueue.h"
#include "threadpool/worker.h"

using namespace std;
using namespace META::Core;

namespace META { namespace Backend {

        typedef void (*publish_fp_t)(const string &apikey, const string &routingkey, const vector<DataSet> &results);

        struct Request {
        public:
            Request() = default;
            Request(string apikey, string routingkey, DataSource &&dataSource)
                    : apikey(move(apikey)), routingkey(move(routingkey)), dataSource(move(dataSource)) {}

            Request(const Request &that) = default;
            Request(Request &&that) = default;

            Request &operator=(const Request &that) {
                this->apikey = that.apikey;
                this->routingkey = that.routingkey;
                this->dataSource = that.dataSource;
                return *this;
            }

            string apikey;
            string routingkey;
            DataSource dataSource;

            DataSource* DataSourcePtr() {
                return &dataSource;
            }
        };

        class Distributor {
        public:
            Distributor(const ComputeUnit *computeUnit, publish_fp_t publish);
            Distributor(const ComputeUnit *computeUnit, uint16_t threads, publish_fp_t publish);

            void distribute(Request request);
            void shutdown();
        private:
            uint16_t threads;
            BlockingQueue workQueue;
            vector<Worker*> workers;

            static map<string, Request> requests;
            static publish_fp_t publish;

            static void acceptResult(const WorkUnit &result);
        };
}}