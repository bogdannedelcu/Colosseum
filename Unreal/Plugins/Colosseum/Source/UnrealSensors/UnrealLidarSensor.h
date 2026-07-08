// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "common/Common.hpp"
#include "GameFramework/Actor.h"
#include "sensors/lidar/LidarSimple.hpp"
#include "NedTransform.h"

// UnrealLidarSensor implementation that uses Ray Tracing in Unreal.
// The implementation uses a model similar to CARLA Lidar implementation.
// Thanks to CARLA folks for this.
class UnrealLidarSensor : public colosseum::LidarSimple
{
public:
    typedef colosseum::ColosseumSettings ColosseumSettings;

public:
    UnrealLidarSensor(const ColosseumSettings::LidarSetting& setting,
                      AActor* actor, const NedTransform* ned_transform);

protected:
    virtual void getPointCloud(const colosseum::Pose& lidar_pose, const colosseum::Pose& vehicle_pose,
                               colosseum::TTimeDelta delta_time, colosseum::vector<colosseum::real_T>& point_cloud, colosseum::vector<int>& segmentation_cloud) override;

private:
    using Vector3r = colosseum::Vector3r;
    using VectorMath = colosseum::VectorMath;

    void createLasers();
    bool shootLaser(const colosseum::Pose& lidar_pose, const colosseum::Pose& vehicle_pose,
                    const float horizontal_angle, const float vertical_angle,
                    const colosseum::LidarSimpleParams& params, Vector3r& point, int& segmentationID);

private:
    AActor* actor_;
    const NedTransform* ned_transform_;

    colosseum::vector<colosseum::real_T> laser_angles_;
    float current_horizontal_angle_ = 0.0f;
};