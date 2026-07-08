// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "sensors/SensorFactory.hpp"
#include <memory>
#include "NedTransform.h"
#include "GameFramework/Actor.h"

class UnrealSensorFactory : public colosseum::SensorFactory
{
public:
    typedef colosseum::ColosseumSettings ColosseumSettings;

public:
    UnrealSensorFactory(AActor* actor, const NedTransform* ned_transform);
    virtual ~UnrealSensorFactory() {}
    void setActor(AActor* actor, const NedTransform* ned_transform);
    virtual std::shared_ptr<colosseum::SensorBase> createSensorFromSettings(
        const ColosseumSettings::SensorSetting* sensor_setting) const override;

private:
    AActor* actor_;
    const NedTransform* ned_transform_;
};
