#pragma once

#include "CoreMinimal.h"

#include "SimMode/SimModeBase.h"
#include "CarPawn.h"
#include "common/Common.hpp"
#include "api/VehicleSimApiBase.hpp"
#include "SimModeCar.generated.h"

UCLASS()
class COLOSSEUM_API ASimModeCar : public ASimModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    virtual void continueForTime(double seconds) override;
    virtual void continueForFrames(uint32_t frames) override;

private:
    typedef colosseum::ClockFactory ClockFactory;
    typedef common_utils::Utils Utils;
    typedef colosseum::TTimePoint TTimePoint;
    typedef colosseum::TTimeDelta TTimeDelta;
    typedef ACarPawn TVehiclePawn;
    typedef colosseum::VehicleSimApiBase VehicleSimApiBase;
    typedef colosseum::VectorMath VectorMath;
    typedef colosseum::Vector3r Vector3r;

private:
    void initializePauseState();

protected:
    virtual void setupClockSpeed() override;
    virtual std::unique_ptr<colosseum::ApiServerBase> createApiServer() const override;
    virtual void getExistingVehiclePawns(TArray<AActor*>& pawns) const override;
    virtual bool isVehicleTypeSupported(const std::string& vehicle_type) const override;
    virtual std::string getVehiclePawnPathName(const ColosseumSettings::VehicleSetting& vehicle_setting) const override;
    virtual PawnEvents* getVehiclePawnEvents(APawn* pawn) const override;
    virtual const common_utils::UniqueValueMap<std::string, APIPCamera*> getVehiclePawnCameras(APawn* pawn) const override;
    virtual void initializeVehiclePawn(APawn* pawn) override;
    virtual std::unique_ptr<PawnSimApi> createVehicleSimApi(
        const PawnSimApi::Params& pawn_sim_api_params) const override;
    virtual colosseum::VehicleApiBase* getVehicleApi(const PawnSimApi::Params& pawn_sim_api_params,
                                                       const PawnSimApi* sim_api) const override;

private:
    std::atomic<float> current_clockspeed_;
    std::atomic<TTimeDelta> pause_period_;
    std::atomic<TTimePoint> pause_period_start_;
    uint32_t targetFrameNumber_;
    std::atomic_bool frame_countdown_enabled_;
    ;
};
