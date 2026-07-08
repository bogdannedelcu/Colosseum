#include "CarPawnApi.h"
#include "ColosseumBlueprintLib.h"

#include "ChaosVehicleManager.h"

CarPawnApi::CarPawnApi(ACarPawn* pawn, const colosseum::Kinematics::State* pawn_kinematics,
                       colosseum::CarApiBase* vehicle_api)
    : pawn_(pawn), pawn_kinematics_(pawn_kinematics), vehicle_api_(vehicle_api)
{
    movement_ = CastChecked<UChaosWheeledVehicleMovementComponent>(pawn->GetVehicleMovement());
}

void CarPawnApi::updateMovement(const colosseum::CarApiBase::CarControls& controls)
{
    last_controls_ = controls;

    if (!controls.is_manual_gear && movement_->GetTargetGear() < 0)
        movement_->SetTargetGear(0, true); //in auto gear we must have gear >= 0
    if (controls.is_manual_gear && movement_->GetTargetGear() != controls.manual_gear)
        movement_->SetTargetGear(controls.manual_gear, controls.gear_immediate);

    movement_->SetThrottleInput(controls.throttle);
    movement_->SetSteeringInput(controls.steering);
    movement_->SetBrakeInput(controls.brake);
    movement_->SetHandbrakeInput(controls.handbrake);
    movement_->SetUseAutomaticGears(!controls.is_manual_gear);
}

colosseum::CarApiBase::CarState CarPawnApi::getCarState() const
{
    colosseum::CarApiBase::CarState state(
        movement_->GetForwardSpeed() / 100, //cm/s -> m/s
        movement_->GetCurrentGear(),
        movement_->GetEngineRotationSpeed(),
        movement_->GetEngineMaxRotationSpeed(),
        last_controls_.handbrake,
        *pawn_kinematics_,
        vehicle_api_->clock()->nowNanos());
    return state;
}

void CarPawnApi::reset()
{
    vehicle_api_->reset();

    last_controls_ = colosseum::CarApiBase::CarControls();
    auto phys_comps = UColosseumBlueprintLib::getPhysicsComponents(pawn_);
    UColosseumBlueprintLib::RunCommandOnGameThread([this, &phys_comps]() {
        for (auto* phys_comp : phys_comps) {
            phys_comp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            phys_comp->SetPhysicsLinearVelocity(FVector::ZeroVector);
            phys_comp->SetSimulatePhysics(false);
        }
        movement_->ResetMoveState();
        movement_->SetActive(false);
        movement_->SetActive(true, true);
        vehicle_api_->setCarControls(colosseum::CarApiBase::CarControls());
        updateMovement(colosseum::CarApiBase::CarControls());
        movement_->ResetVehicleState();
        //auto pv = movement_->PVehicle;
        //if (pv) {
        //    pv->mWheelsDynData.setToRestState();
        //}
        //auto pvd = movement_->PVehicleDrive;
        //if (pvd) {
        //    pvd->mDriveDynData.setToRestState();
        //}
    },
                                             true);

    UColosseumBlueprintLib::RunCommandOnGameThread([this, &phys_comps]() {
        for (auto* phys_comp : phys_comps)
            phys_comp->SetSimulatePhysics(true);
    },
                                             true);
}

void CarPawnApi::update()
{
    vehicle_api_->updateCarState(getCarState());
    vehicle_api_->update();
}

CarPawnApi::~CarPawnApi() = default;
