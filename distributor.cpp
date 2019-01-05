//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include <thread>
#include "distributor.h"

using namespace META::Core;

namespace META { namespace Backend {

        std::map<std::string, Request> Distributor::requests;
        publish_fp_t Distributor::publish;

        Distributor::Distributor(const ComputeUnit *computeUnit, publish_fp_t publish) {
            this->threads = static_cast<uint16_t>(std::thread::hardware_concurrency());
            Distributor::publish = *publish;

            for(uint16_t i = 0; i < this->threads; i++) {
                workers.push_back(new Worker(i, &workQueue, computeUnit, acceptResult));
            }
        }

        Distributor::Distributor(const ComputeUnit *computeUnit, uint16_t threads, publish_fp_t publish) {
            this->threads = threads;
            Distributor::publish = *publish;

            for(uint16_t i = 0; i < this->threads; i++) {
                workers.push_back(new Worker(i, &workQueue, computeUnit, acceptResult));
            }
        }

        void Distributor::distribute(Request request) {
            requests.emplace(make_pair(request.routingkey, request));
            //requests[request.routingkey] = request;
            workQueue.push(request.routingkey, request.DataSourcePtr());
        }

        void Distributor::acceptResult(const WorkUnit &result) {
            Request &r = requests[result.key];
            r.dataSource.appendResult(result.data);
            if(r.dataSource.hasAllResults()) {
                // datasource finished // send it back

                r.dataSource.markFinished();
                Distributor::publish(r.apikey, r.routingkey, r.dataSource.getResults());
                requests.erase(result.key);
            }
        }

        void Distributor::shutdown() {
            workQueue.shutdown();
            for(Worker *w : workers)
                w->join();
        }

}}