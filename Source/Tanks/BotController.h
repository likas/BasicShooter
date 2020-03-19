// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bot.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "Containers/Queue.h"
#include "BotController.generated.h"

/**
 * Controller for AI player
 */
UCLASS()
class TANKS_API ABotController : public AAIController
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	void BeginPlay() override;

protected:
	// Our pawn, pre-cast to a Zombie. Will be NULL if the pawn is NULL or is not a Zombie.
	UPROPERTY(BlueprintReadOnly, Category = "Bot")
		ABot* PawnAsBot;

private:
	//Currently cannot be exposed to Blueprints using UPROPERTY. There are some workarounds
	//but let's leave it be for now
		TQueue<FVector> TravelPoints;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "BotController")
		FVector Destination;

	UPROPERTY()
		TArray<AActor*> Waypoints;

	UFUNCTION(BlueprintPure)
		FVector GetNewMovePoint();
};
