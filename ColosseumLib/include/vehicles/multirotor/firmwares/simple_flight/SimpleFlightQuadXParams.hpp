// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef msr_colosseum_vehicles_SimpleFlightQuadX_hpp
#define msr_colosseum_vehicles_SimpleFlightQuadX_hpp

#include "vehicles/multirotor/firmwares/simple_flight/SimpleFlightApi.hpp"
#include "vehicles/multirotor/MultiRotorParams.hpp"
#include "common/ColosseumSettings.hpp"
#include "sensors/SensorFactory.hpp"

namespace colosseum
{

    class SimpleFlightQuadXParams : public MultiRotorParams
    {
    public:
        SimpleFlightQuadXParams(const ColosseumSettings::VehicleSetting* vehicle_setting, std::shared_ptr<const SensorFactory> sensor_factory)
            : vehicle_setting_(vehicle_setting), sensor_factory_(sensor_factory)
        {
        }

        virtual ~SimpleFlightQuadXParams() = default;

        virtual std::unique_ptr<MultirotorApiBase> createMultirotorApi() override
        {
            return std::unique_ptr<MultirotorApiBase>(new SimpleFlightApi(this, vehicle_setting_));
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

    private:
        const ColosseumSettings::VehicleSetting* vehicle_setting_; //store as pointer because of derived classes
        std::shared_ptr<const SensorFactory> sensor_factory_;
    };
}
#endif
