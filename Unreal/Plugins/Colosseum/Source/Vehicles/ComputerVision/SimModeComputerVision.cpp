// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "SimModeComputerVision.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

#include "ColosseumBlueprintLib.h"
#include "common/ColosseumSettings.hpp"
#include "PawnSimApi.h"
#include "ColosseumBlueprintLib.h"
#include "common/Common.hpp"
#include "common/EarthUtils.hpp"
#include "api/VehicleSimApiBase.hpp"
#include "common/ColosseumSettings.hpp"
#include "physics/Kinematics.hpp"
#include "api/RpcLibServerBase.hpp"

std::unique_ptr<colosseum::ApiServerBase> ASimModeComputerVision::createApiServer() const
{
#ifdef COLOSSEUM_NO_RPC
    return ASimModeBase::createApiServer();
#else
    return std::unique_ptr<colosseum::ApiServerBase>(new colosseum::RpcLibServerBase(
        getApiProvider(), getSettings().api_server_address, getSettings().api_port));
#endif
}

void ASimModeComputerVision::getExistingVehiclePawns(TArray<AActor*>& pawns) const
{
    UColosseumBlueprintLib::FindAllActor<TVehiclePawn>(this, pawns);
}

bool ASimModeComputerVision::isVehicleTypeSupported(const std::string& vehicle_type) const
{
    return vehicle_type == colosseum::ColosseumSettings::kVehicleTypeComputerVision;
}

std::string ASimModeComputerVision::getVehiclePawnPathName(const ColosseumSettings::VehicleSetting& vehicle_setting) const
{
    //decide which derived BP to use
    std::string pawn_path = vehicle_setting.pawn_path;
    if (pawn_path == "")
        pawn_path = "DefaultComputerVision";

    return pawn_path;
}

PawnEvents* ASimModeComputerVision::getVehiclePawnEvents(APawn* pawn) const
{
    return static_cast<TVehiclePawn*>(pawn)->getPawnEvents();
}
const common_utils::UniqueValueMap<std::string, APIPCamera*> ASimModeComputerVision::getVehiclePawnCameras(
    APawn* pawn) const
{
    return static_cast<const TVehiclePawn*>(pawn)->getCameras();
}
void ASimModeComputerVision::initializeVehiclePawn(APawn* pawn)
{
    static_cast<TVehiclePawn*>(pawn)->initializeForBeginPlay();
}

std::unique_ptr<PawnSimApi> ASimModeComputerVision::createVehicleSimApi(
    const PawnSimApi::Params& pawn_sim_api_params) const
{
    auto vehicle_sim_api = std::unique_ptr<PawnSimApi>(new PawnSimApi(pawn_sim_api_params));
    vehicle_sim_api->initialize();
    vehicle_sim_api->reset();
    return vehicle_sim_api;
}

colosseum::VehicleApiBase* ASimModeComputerVision::getVehicleApi(const PawnSimApi::Params& pawn_sim_api_params,
                                                                   const PawnSimApi* sim_api) const
{
    //we don't have real vehicle so no vehicle API
    return nullptr;
}
