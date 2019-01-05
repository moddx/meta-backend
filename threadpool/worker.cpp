//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include "worker.h"

#include <string>
#include <iostream>
#include <sstream>

using namespace META::Core;

namespace META { namespace Backend {

        /**
         * Creates a new Worker object in an uninitialized state.
         * Use the init() function to finish the initialization and to start the worker.
         *
         * @tparam T
         * @tparam std::function<void(WorkUnit)>
         */
        Worker::Worker() {
            uninitialized = true;
        }

        /**
         * Creates a new Worker object.
         * The worker starts working immediately.
         *
         * @tparam T
         * @tparam Callback
         * @param queue
         * @param computeUnit
         * @param callback
         */
        Worker::Worker(uint16_t id, BlockingQueue *queue, const ComputeUnit *computeUnit,
                                    std::function<void(WorkUnit)> callback) {
            this->id = id;
            this->queue = queue;
            this->computeUnit = computeUnit;
            this->callback = callback;
            t = std::thread(run, id, queue, computeUnit, callback);
        }

        /**
         * Finishes the initialization of an uninitialized Worker, created by the empty
         * constructor Worker(). This call also starts the Worker.
         * @tparam T
         * @tparam Callback
         * @param queue
         * @param computeUnit
         * @param callback
         */
        void Worker::init(uint16_t id, BlockingQueue *queue, const ComputeUnit *computeUnit,
                                       std::function<void(WorkUnit)> callback) {
            if(!uninitialized) throw "Can't reinitialize a Worker. Called init() on initialized Worker object!";
            this->id = id;
            this->uninitialized = false;
            this->queue = queue;
            this->computeUnit = computeUnit;
            this->callback = callback;
            t = std::thread(run, id, queue, computeUnit, callback);
        }

        void Worker::join() {
            if(uninitialized)
                return;
            t.join();
            std::cout << "Joining Worker #" << id << "\n";
        }

        void Worker::run(uint16_t id, BlockingQueue *queue, const ComputeUnit *computeUnit, std::function<void(WorkUnit)> callback) {
            while (queue->keepWorking()) {
                WorkUnit work = queue->pop();  // blocks on empty queue
                if (work.data.isUninitialized()) continue;
                ComputeData *result = computeUnit->compute(work.data.value);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // todo remove

                std::stringstream buf;
                buf << "Worker #" << id << " has result: " << result->toString() << "\n";
                std::cout << buf.str();

                // store the result somewhere..
                work.data.value = result;
                callback(work);
            }

            std::cout << "Stopping Worker #" + std::to_string(id) + " ..\n";
        }

}}