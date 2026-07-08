// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef colosseum_IGeoFence_hpp
#define colosseum_IGeoFence_hpp

#include "common/Common.hpp"

namespace colosseum
{

    class IGeoFence
    {
    public:
        virtual void setBoundry(const Vector3r& origin, float xy_length, float max_z, float min_z) = 0;
        virtual void checkFence(const Vector3r& cur_loc, const Vector3r& dest_loc,
                                bool& in_fence, bool& allow) = 0;
        virtual string toString() const = 0;

        virtual ~IGeoFence(){};
    };
}
#endif
