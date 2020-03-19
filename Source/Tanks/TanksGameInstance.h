// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TanksGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API UTanksGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "TankScore")
        int32 GetTankScore() const;

    UFUNCTION(BlueprintPure, Category = "BotScore")
        int32 GetBotScore() const;

    UFUNCTION(BlueprintCallable, Category = "TankScore")
        void SetTankScore();

    UFUNCTION(BlueprintCallable, Category = "BotScore")
        void SetBotScore();

private:
	int32 TankScore;
	int32 BotScore;
	
};
