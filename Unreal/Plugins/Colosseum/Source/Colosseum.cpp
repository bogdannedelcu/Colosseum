// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Colosseum.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

class FColosseum : public IModuleInterface
{
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FColosseum, Colosseum)

void FColosseum::StartupModule()
{
    //plugin startup
    UE_LOG(LogTemp, Log, TEXT("StartupModule: Colosseum plugin"));
}

void FColosseum::ShutdownModule()
{
    //plugin shutdown
}