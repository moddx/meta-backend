//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include <thread>
#include <functional>
#include "computeunit.h"
#include "datasource.h"
#include "blockingqueue.h"
#include "workunit.h"

using namespace META::Core;

namespace META { namespace Backend {

        class BlockingQueue;

        class Worker {
        public:
            Worker();
            Worker(uint16_t id, BlockingQueue *queue, const ComputeUnit *computeUnit, std::function<void(WorkUnit)> callback);
            void init(uint16_t id, BlockingQueue *queue, const ComputeUnit *computeUnit, std::function<void(WorkUnit)> callback);
            void join();

            // Delete copy and move constructors/operators, because copying a running worker
            // does not make any sense.
            Worker(const Worker& other) = delete;
            Worker(const Worker&& other) = delete;
            Worker& operator= (const Worker& other) = delete;
            Worker& operator= (Worker&& other) = delete;
            // ~Worker() = delete;
        private:
            uint16_t id;
            bool uninitialized = false;
            std::function<void(WorkUnit)> callback;
            BlockingQueue *queue;
            const ComputeUnit *computeUnit;
            std::thread t;
            static void run(uint16_t id, BlockingQueue *queue, const ComputeUnit *computeUnit, std::function<void(WorkUnit)> callback);//, std::function<void(WorkUnit)> callback);
        };

}}