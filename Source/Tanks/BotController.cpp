// Fill out your copyright notice in the Description page of Project Settings.

#include "Tanks.h"
#include "Bot.h"
#include "BotController.h"

void ABotController::Tick(float DeltaTime)
{
	if (PawnAsBot)
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnAsBot"));
		if (AActor* Target = PawnAsBot->GetTarget())
		{
			// We do have a target. Shamble toward it and attempt violence!
			FVector DirectionToTarget = (Target->GetActorLocation() - PawnAsBot->GetActorLocation()).GetSafeNormal2D();
			float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsBot->GetActorForwardVector());
			float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsBot->GetActorRightVector());
			//Relative angle to add to where we faacing to be where we want to face
			float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);

			if (PawnAsBot->BotAIShouldAttack())
			{
				UE_LOG(LogTemp, Warning, TEXT("We think we should attack"));
				PawnAsBot->AddAttackInput();
			}
			else //delete this 'else' if we want to move when attacking
			{
				UE_LOG(LogTemp, Warning, TEXT("We move"));
				// Move faster when facing toward the target so that we turn more accurately/don't orbit.
				PawnAsBot->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), FMath::GetMappedRangeValueClamped(FVector2D(-0.707f, 0.707f), FVector2D(0.0f, 1.0f), DotToTarget));
				// Attempt the entire turn in one frame. The Zombie itself will cap this, we're only expressing our desired turn amount here.
				PawnAsBot->AddRotationInput(DeltaYawDesired);
			}
		}
		else 
		{ 
			UE_LOG(LogTemp, Warning, TEXT("NoTarget"));
		}

	}
}

void ABotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("Posessed the pawn"));
	PawnAsBot = Cast<ABot>(GetPawn());
}

void ABotController::OnUnPossess()
{
	Super::OnUnPossess();
	PawnAsBot = nullptr;
}