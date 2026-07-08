// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "common/Common.hpp"
#include "GameFramework/Actor.h"
#include "sensors/distance/DistanceSimple.hpp"
#include "NedTransform.h"

class UnrealDistanceSensor : public colosseum::DistanceSimple
{
public:
    typedef colosseum::ColosseumSettings ColosseumSettings;

public:
    UnrealDistanceSensor(const ColosseumSettings::DistanceSetting& setting,
                         AActor* actor, const NedTransform* ned_transform);

protected:
    virtual colosseum::real_T getRayLength(const colosseum::Pose& pose) override;

private:
    using Vector3r = colosseum::Vector3r;
    using VectorMath = colosseum::VectorMath;

private:
    AActor* actor_;
    const NedTransform* ned_transform_;
};