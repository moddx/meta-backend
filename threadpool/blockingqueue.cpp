//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include "blockingqueue.h"

using namespace META::Core;

namespace META { namespace Backend {

        void BlockingQueue::push(const WorkUnit work) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                queue.push(work);
                queued_items++;
            }

            // notify
            dataAvailable.notify_one();
        }

        void BlockingQueue::push(const std::string &key, DataSource *dataSource) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                while (dataSource->hasNextElement()) {
                    DataSet element = dataSource->getNextElement();

                    WorkUnit work(key, element);
                    queue.push(work);
                    queued_items++;
                }
            }

            dataAvailable.notify_all();
        }

        const WorkUnit BlockingQueue::pop() {
            std::unique_lock<std::mutex> lock(mutex);
            // wait till data is available
            while (queued_items <= 0) {
                if(forceExit) return WorkUnit();     // return _anything_ to release workers
                dataAvailable.wait(lock);   // temporarily frees mutex; until resume
            }

            if(forceExit) return WorkUnit();         // return _anything_ to release workers

            WorkUnit top = queue.front();
            queue.pop();
            queued_items--;

            return top;
        }

        void BlockingQueue::shutdown() {
            forceExit = true;
            dataAvailable.notify_all();
        }

        bool BlockingQueue::keepWorking() {
            return !forceExit;
        }

}}