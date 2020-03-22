// Fill out your copyright notice in the Description page of Project Settings.

#include "BotController.h"
#include "Tanks.h"
#include "Bot.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "PaperSpriteComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
#include "Engine/TargetPoint.h"


void ABotController::BeginPlay()
{
	Super::BeginPlay();
	Destination = PawnAsBot->GetActorLocation();
}

void ABotController::Tick(float DeltaTime)
{
	/* Check first if we're blocked */
	FVector NormalToObstacle;
	bool bBlockedByObstacle = PawnAsBot->BotAIObstacleInTheWay(NormalToObstacle);

	//If we "see player"
	if (PawnAsBot->BotAITargetInSight())
	{
		Destination = GetNewMovePoint();
		/* Draw debug sphere where the destination is, but only if we're in PIE */
		if (GetWorld()->IsPlayInEditor())
		{
			DrawDebugSphere(GetWorld(), Destination, 10.f, 12, FColor(255,0,0), false, 30.f);
		}
		/* 
		Bot AI thinks we should attack, if the trajectory of our sight is intercepting with a player's body sprite
		*/
		if (PawnAsBot->BotAIShouldAttack()) 
		{
			PawnAsBot->AddAttackInput();
		}

		FVector DestinationVector;
		if (bBlockedByObstacle)
		{
			DestinationVector = NormalToObstacle.RightVector;
		}
		else
		{
			DestinationVector = PawnAsBot->GetActorLocation() - PawnAsBot->GetTarget()->GetActorLocation();
			PawnAsBot->AddMovementInput(FVector(DestinationVector.GetSafeNormal().X, DestinationVector.GetSafeNormal().Y, 0.f));
		}

		//float DotToTarget = FVector::DotProduct(DestinationVector.GetSafeNormal(), PawnAsBot->GetActorForwardVector());
		float SidewaysDotToTarget = FVector::DotProduct(DestinationVector.GetSafeNormal(), PawnAsBot->GetActorRightVector());

		PawnAsBot->AddRotationInput(SidewaysDotToTarget * 10);
	}
	else 
	{
		// If we don't see player (move randomly)
		FVector DestinationVector;
		if (FVector::DistXY(PawnAsBot->GetActorLocation(), Destination) <= 3.f || bBlockedByObstacle) 
		{
			Destination = GetNewMovePoint();
			/* Draw debug sphere where the destination is, but only if we're in PIE */
			if (GetWorld()->IsPlayInEditor()) 
			{
				DrawDebugSphere(GetWorld(), Destination, 10.f, 12, FColor(255, 0, 0), false, 30.f);
				DrawDebugDirectionalArrow(GetWorld(), PawnAsBot->GetActorLocation(), Destination, 30.f, FColor(0, 255, 0), false, 100.f);
			}
		}
		DestinationVector = (Destination - PawnAsBot->GetActorLocation()).GetSafeNormal();

		float DotToTarget = FVector::DotProduct(DestinationVector, PawnAsBot->GetActorForwardVector());
		float SidewaysDotToTarget = FVector::DotProduct(DestinationVector, PawnAsBot->GetActorRightVector());
		float SupposedToBeAngle = FMath::RadiansToDegrees( FMath::Atan2(SidewaysDotToTarget, DotToTarget) );
		UE_LOG(LogTemp, Log, TEXT("Angle is %f"), SupposedToBeAngle);

		//float Angle = PawnAsBot->GetActorForwardVector().Rotation().Yaw - DestinationVector.Rotation().Yaw;
		//float Acos = FMath::RadiansToDegrees( FMath::Acos(DotToTarget) );
	
		//if (FMath::Sign(DotToTarget) != FMath::Sign(SidewaysDotToTarget) && FMath::IsNearlyZero(DotToTarget) && FMath::IsNearlyZero(SidewaysDotToTarget)) { SidewaysDotToTarget += 0.5f; }

		//Rotate to target
		PawnAsBot->AddRotationInput(SupposedToBeAngle);

		//If not blocked and almost facing target
		if (FMath::Abs(SupposedToBeAngle) < 0.1f)
		{
			if (!bBlockedByObstacle) {
				PawnAsBot->AddMovementInput(FVector(DestinationVector.X, DestinationVector.Y, 0.f));
			}
		}
	}
}

FVector ABotController::GetNewMovePoint()
{
	FVector NewMovePoint;
	/* if we won't set this for some reason, we'll return our current location. For safety! */
	NewMovePoint = PawnAsBot->GetActorLocation();

	if (TravelPoints.IsEmpty() || (TravelPoints.Dequeue(NewMovePoint) == false)) {

		UE_LOG(LogTemp, Log, TEXT("Getting new targets..."));

		auto NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		UNavigationPath* TravelPath;

		/*
		TODO This function seems to be outdated. Look up for the replacement
		OR more likely remove this at all and just generate random direction and rotation
		AND handle navigation by collision management
		*/
		auto randPoint = NavSystem->GetRandomPointInNavigableRadius(GWorld, PawnAsBot->GetActorLocation(),
			FMath::RandRange(100, 1000));

		//Equalize Z part so we don't try to move in 3D space (since we're 2D game)
		randPoint.Z = PawnAsBot->GetActorLocation().Z;

		TravelPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), PawnAsBot->GetActorLocation(), randPoint);
		//Populate TravelPoints
		if (TravelPath)
		{
			UE_LOG(LogTemp, Log, TEXT("PathPointsCounter: %d"), TravelPath->PathPoints.Num());

			/*
			i is 2 because first point is our current position and the second we return immediately
			Will rename this function to "GenerateNewPathPoints"
			and make another one named "GetNewMovePoint" to return points from the array populated from there
			*/
			for (int i = 2; i < TravelPath->PathPoints.Num(); ++i)
			{
				TravelPath->PathPoints[i].Z = PawnAsBot->GetActorLocation().Z;
				TravelPoints.Enqueue(TravelPath->PathPoints[i]);
				UE_LOG(LogTemp, Log, TEXT("%d. Added Random Location: %s"), i, *TravelPath->PathPoints[i].ToString());
			}
		}
		if (TravelPath->PathPoints.Num() > 1)
		{
			NewMovePoint = TravelPath->PathPoints[1];
		}
	}
	else
	{
		bool bDequeueSuccessful = TravelPoints.Dequeue(NewMovePoint);
		if (!bDequeueSuccessful)
		{
			/* This sometimes happens for unknown (yet) reason */
			UE_LOG(LogTemp, Warning, TEXT("Dequeue failed"));
		}
	}
	return NewMovePoint;
}

void ABotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PawnAsBot = Cast<ABot>(GetPawn());
}

void ABotController::OnUnPossess()
{
	Super::OnUnPossess();
	PawnAsBot = nullptr;
}

