// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "UnrealSensorFactory.h"
#include "UnrealSensors/UnrealDistanceSensor.h"
#include "UnrealSensors/UnrealLidarSensor.h"

UnrealSensorFactory::UnrealSensorFactory(AActor* actor, const NedTransform* ned_transform)
{
    setActor(actor, ned_transform);
}

std::shared_ptr<colosseum::SensorBase> UnrealSensorFactory::createSensorFromSettings(
    const ColosseumSettings::SensorSetting* sensor_setting) const
{
    using SensorBase = colosseum::SensorBase;

    switch (sensor_setting->sensor_type) {
    case SensorBase::SensorType::Distance:
        return std::shared_ptr<UnrealDistanceSensor>(new UnrealDistanceSensor(
            *static_cast<const ColosseumSettings::DistanceSetting*>(sensor_setting), actor_, ned_transform_));
    case SensorBase::SensorType::Lidar:
        return std::shared_ptr<UnrealLidarSensor>(new UnrealLidarSensor(
            *static_cast<const ColosseumSettings::LidarSetting*>(sensor_setting), actor_, ned_transform_));
    default:
        return colosseum::SensorFactory::createSensorFromSettings(sensor_setting);
    }
}

void UnrealSensorFactory::setActor(AActor* actor, const NedTransform* ned_transform)
{
    actor_ = actor;
    ned_transform_ = ned_transform;
}
