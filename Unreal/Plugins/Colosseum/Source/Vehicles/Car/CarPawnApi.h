#pragma once

#include "vehicles/car/api/CarApiBase.hpp"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "physics/Kinematics.hpp"
#include "CarPawn.h"

class CarPawnApi
{
public:
    typedef colosseum::ImageCaptureBase ImageCaptureBase;

    CarPawnApi(ACarPawn* pawn, const colosseum::Kinematics::State* pawn_kinematics,
               colosseum::CarApiBase* vehicle_api);

    void updateMovement(const colosseum::CarApiBase::CarControls& controls);

    colosseum::CarApiBase::CarState getCarState() const;

    void reset();
    void update();

    virtual ~CarPawnApi();

private:
    UChaosWheeledVehicleMovementComponent* movement_;
    colosseum::CarApiBase::CarControls last_controls_;
    ACarPawn* pawn_;
    const colosseum::Kinematics::State* pawn_kinematics_;
    colosseum::CarApiBase* vehicle_api_;
};
