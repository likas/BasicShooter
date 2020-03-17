// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bot.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "Containers/Queue.h"
#include "BotController.generated.h"

class ABot;
/**
 * 
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
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	//ABotController(const FPostConstructInitializeProperties& PCIP);

protected:
	// Our pawn, pre-cast to a Zombie. Will be NULL if the pawn is NULL or is not a Zombie.
	UPROPERTY(BlueprintReadOnly, Category = "Bot")
		ABot* PawnAsBot;

	//Distance we want to maintain from the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot")
		float Distance;
private:
	//Currently cannot be exposed to Blueprints using UPROPERTY. There are some workarounds
	//but let's leave it be for now
	TQueue<FVector> TravelPoints;
	UPROPERTY()
	FVector Destination;

	UPROPERTY()
		TArray<AActor*> Waypoints;

	UFUNCTION(BlueprintPure)
		ATargetPoint* GetRandomWaypoint() const;

	UFUNCTION(BlueprintPure)
		FVector GetNewMovePoint();

	UFUNCTION(BlueprintCallable)
		void GoToRandomWaypoint();
};
