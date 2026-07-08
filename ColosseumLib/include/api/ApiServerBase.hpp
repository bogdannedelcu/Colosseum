// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef colosseum_ApiServerBase_hpp
#define colosseum_ApiServerBase_hpp

#include <functional>
#include "common/Common.hpp"

namespace colosseum
{

    class ApiServerBase
    {
    public:
        virtual void start(bool block, std::size_t thread_count) = 0;
        virtual void stop() = 0;

        virtual ~ApiServerBase() = default;
    };
}
#endif
