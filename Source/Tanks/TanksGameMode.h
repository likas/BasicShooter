// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TanksGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATanksGameMode : public AGameMode
{
	GENERATED_BODY()
public:

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    /* The widget class of hud */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Score", Meta = (BlueprintProtected = "true"))
    TSubclassOf<class UUserWidget> HUDWigdetClass;

    /* The actual instance of the HUD */
    UPROPERTY()
    class UUserWidget* CurrentWidget;
};
