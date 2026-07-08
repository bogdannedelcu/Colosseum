// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameUserSettings.h"
#include "ColosseumGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COLOSSEUM_API AColosseumGameMode : public AGameModeBase
{
public:
    GENERATED_BODY()

    virtual void StartPlay() override;

    AColosseumGameMode(const FObjectInitializer& ObjectInitializer);

    //private:
    //UGameUserSettings* GetGameUserSettings();
};
