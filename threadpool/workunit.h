//
// Created by Matthias Ervens on 18/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include <string>
#include <datasource.h>

using namespace META::Core;

namespace META { namespace Backend {

        struct WorkUnit {
            WorkUnit() {
                key = "";
                DataSet set;
                data = set;
            }

            WorkUnit(std::string key, DataSet data) {
                this->key = std::move(key);
                this->data = std::move(data);
            }

            std::string key;
            DataSet data;
        };
}}