// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef msr_colosseum_vehicles_ArduCopter_hpp
#define msr_colosseum_vehicles_ArduCopter_hpp

#include "vehicles/multirotor/firmwares/arducopter/ArduCopterApi.hpp"
#include "vehicles/multirotor/MultiRotorParams.hpp"
#include "common/ColosseumSettings.hpp"
#include "sensors/SensorFactory.hpp"

namespace colosseum
{

    class ArduCopterParams : public MultiRotorParams
    {
    public:
        ArduCopterParams(const ColosseumSettings::MavLinkVehicleSetting& vehicle_setting, std::shared_ptr<const SensorFactory> sensor_factory)
            : sensor_factory_(sensor_factory)
        {
            connection_info_ = getConnectionInfo(vehicle_setting);
        }

        virtual ~ArduCopterParams() = default;

        virtual std::unique_ptr<MultirotorApiBase> createMultirotorApi() override
        {
            return std::unique_ptr<MultirotorApiBase>(new ArduCopterApi(this, connection_info_));
        }

    protected:
        virtual void setupParams() override
        {
            auto& params = getParams();

            // Use connection_info_.model for the model name, see Px4MultiRotorParams for example

            // Only Generic for now
            setupFrameGenericQuad(params);
        }

        virtual const SensorFactory* getSensorFactory() const override
        {
            return sensor_factory_.get();
        }

        static const ColosseumSettings::MavLinkConnectionInfo& getConnectionInfo(const ColosseumSettings::MavLinkVehicleSetting& vehicle_setting)
        {
            return vehicle_setting.connection_info;
        }

    private:
        ColosseumSettings::MavLinkConnectionInfo connection_info_;
        std::shared_ptr<const SensorFactory> sensor_factory_;
    };
}
#endif
