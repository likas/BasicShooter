// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bot.h"
#include "CoreMinimal.h"
#include "AIController.h"
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

protected:
	// Our pawn, pre-cast to a Zombie. Will be NULL if the pawn is NULL or is not a Zombie.
	UPROPERTY(BlueprintReadOnly, Category = "Bot")
		ABot* PawnAsBot;
};
