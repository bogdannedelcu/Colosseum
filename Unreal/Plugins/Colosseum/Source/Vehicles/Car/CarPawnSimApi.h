#pragma once

#include "CoreMinimal.h"
#include "ChaosWheeledVehicleMovementComponent.h"

#include "CarPawn.h"
#include "CarPawnApi.h"
#include "PawnEvents.h"
#include "PawnSimApi.h"
#include "vehicles/car/api/CarApiBase.hpp"
#include "physics/Kinematics.hpp"
#include "common/Common.hpp"
#include "common/CommonStructs.hpp"
#include "vehicles/car/CarApiFactory.hpp"

class CarPawnSimApi : public PawnSimApi
{
public:
    typedef colosseum::Utils Utils;
    typedef colosseum::StateReporter StateReporter;
    typedef colosseum::UpdatableObject UpdatableObject;
    typedef colosseum::Pose Pose;

public:
    virtual void initialize() override;
    virtual ~CarPawnSimApi() = default;

    //VehicleSimApiBase interface
    //implements game interface to update pawn
    CarPawnSimApi(const Params& params,
                  const colosseum::CarApiBase::CarControls& keyboard_controls);

    virtual void update() override;
    virtual void reportState(StateReporter& reporter) override;

    virtual std::string getRecordFileLine(bool is_header_line) const override;

    virtual void updateRenderedState(float dt) override;
    virtual void updateRendering(float dt) override;

    colosseum::CarApiBase* getVehicleApi() const
    {
        return vehicle_api_.get();
    }

    virtual colosseum::VehicleApiBase* getVehicleApiBase() const override
    {
        return vehicle_api_.get();
    }

protected:
    virtual void resetImplementation() override;

private:
    void updateCarControls();

private:
    std::unique_ptr<colosseum::CarApiBase> vehicle_api_;
    std::unique_ptr<CarPawnApi> pawn_api_;
    std::vector<std::string> vehicle_api_messages_;

    //storing reference from pawn
    const colosseum::CarApiBase::CarControls& keyboard_controls_;

    colosseum::CarApiBase::CarControls joystick_controls_;
    colosseum::CarApiBase::CarControls current_controls_;
};
