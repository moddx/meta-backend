//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

#include "datasource.h"
#include "workunit.h"

using namespace META::Core;

namespace META { namespace Backend {

        class BlockingQueue {
        public:
            void push(WorkUnit work);
            void push(const std::string &key, DataSource *dataSource);
            const WorkUnit pop();
            void shutdown();
            bool keepWorking();
        private:
            bool forceExit = false;
            int queued_items = 0;
            std::condition_variable dataAvailable;
            std::mutex mutex;
            std::queue<WorkUnit> queue;
        };

}}