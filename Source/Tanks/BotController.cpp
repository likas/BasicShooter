// Fill out your copyright notice in the Description page of Project Settings.

#include "BotController.h"
#include "Tanks.h"
#include "Bot.h"
//#include "Navigation/CrowdFollowingComponent.h"
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
	//MoveToLocation(GetNewMovePoint());
}

FVector ABotController::GetNewMovePoint() 
{
	FVector Return;
	if (TravelPoints.IsEmpty() || (TravelPoints.Dequeue(Return) == false)) {
		UE_LOG(LogTemp, Log, TEXT("Getting new targets..."));
		auto NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		UNavigationPath* TravelPath;

		auto randPoint = NavSystem->GetRandomPointInNavigableRadius(GWorld, PawnAsBot->GetActorLocation(), 1000);

		randPoint.Z = PawnAsBot->GetActorLocation().Z;
		//UE_LOG(LogTemp, Warning, TEXT("New Random Location: %f %f %f"), randPoint.X, randPoint.Y, randPoint.Z);

		TravelPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), PawnAsBot->GetActorLocation(), randPoint);
		//Populate TravelPoints
		if (TravelPath)
		{
			//(TravelPath->PathPoints).Num();
			UE_LOG(LogTemp, Log, TEXT("PathPointsCounter: %d"), TravelPath->PathPoints.Num());
			for (int i = 1; i < TravelPath->PathPoints.Num(); ++i)
			{
				TravelPath->PathPoints[i].Z = PawnAsBot->GetActorLocation().Z;
				//DrawDebugSphere(GetWorld(), TravelPath->PathPoints[i], 10.f, 12, FColor(255, 0, 0), false, 100.f);
				TravelPoints.Enqueue(TravelPath->PathPoints[i]);
				//UE_LOG(LogTemp, Warning, TEXT("%d. Added Random Location: %f %f %f"), i, TravelPath->PathPoints[i].X, TravelPath->PathPoints[i].Y, TravelPath->PathPoints[i].Z);
				UE_LOG(LogTemp, Warning, TEXT("%d. Added Random Location: %s"), i, *TravelPath->PathPoints[i].ToString());
			}
			
		}
	}
	else return Return;
	UE_LOG(LogTemp, Log, TEXT("Dequeue successful: %d"), TravelPoints.Dequeue(Return) ? 1 : 0);
	return Return;
}

void ABotController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	//NavigationHandle::PointCheck()
	//MoveToLocation(FVector());
	if (true)
	{
		//FVector* RandomLocation = FVector(FVector::ZeroVector);
		//bool bIsFound = NavSystem->GetRandomPointInNavigableRadius(RandomLocation);

		
		UE_LOG(LogTemp, Warning, TEXT("TankZ: %f %f %f "), PawnAsBot->GetActorLocation().X, PawnAsBot->GetActorLocation().Y, PawnAsBot->GetActorLocation().Z);


		auto NewMovePoint = GetNewMovePoint();
		/*
		UWorld* World = GetWorld();
		if (AActor* NewProjectile = World->SpawnActor<AActor>(APawn::StaticClass(), randPoint, PawnAsBot->GetActorRotation()) )
		{
			UE_LOG(LogTemp, Log, TEXT("Sphere spawned"));
			NewProjectile->SetActorLocation(randPoint);
			//NewProjectile->SetActorRotation(Rot);
		}
		*/
		//DrawDebugSphere(GetWorld(), NewMovePoint, 10.f, 12, FColor(255, 0, 0), false, 100.f);
		//MoveToLocation(NewMovePoint);

		//First, we need to rotate	w
			if (true) //old rotation
			{
				FVector DirectionToTarget = UKismetMathLibrary::GetDirectionUnitVector(PawnAsBot->GetActorLocation(), NewMovePoint);
				FVector DirectionToTargetOld = (NewMovePoint.GetSafeNormal2D() - PawnAsBot->GetActorLocation().GetSafeNormal2D());
				FRotator Rotation = DirectionToTarget.Rotation();
				UE_LOG(LogTemp, Warning, TEXT("Direction to target: %f %f %f"), DirectionToTarget.X, DirectionToTarget.Y, DirectionToTarget.Z);
				//PawnAsBot->AddActorLocalRotation(FRotator(0.f, Rotation.Yaw, 0.f));

				PawnAsBot->SetActorRotation(FRotator(0.f, Rotation.Yaw, 0.f));
			}
		/*
		
		float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsBot->GetActorRightVector());
		float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsBot->GetActorForwardVector());
		float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);
		UE_LOG(LogTemp, Warning, TEXT("DesiredRotation: %f"), DeltaYawDesired);
		//PawnAsBot->AddRotationInput(DesiredRotation);
		FRotator NewRotation = FRotator(0.f, DeltaYawDesired, 0.f);
		FQuat QuatRotation = FQuat(NewRotation);
		PawnAsBot->AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		//PawnAsBot->SetActorRotation(PawnAsBot->GetActorRotation() +  FRotator(0.f, DeltaYawDesired, 0.f));
		*/
		MoveToLocation(NewMovePoint);
	}
	//GoToRandomWaypoint();
}

void ABotController::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("Distance between Dest and Loc: %f"), FVector::DistXY(PawnAsBot->GetActorLocation(), Destination));
	//Give movement input
	FVector DestinationVector;
	if (FVector::DistXY(PawnAsBot->GetActorLocation(), Destination) <= 10.f) {
		Destination = GetNewMovePoint();
		
		DrawDebugSphere(GetWorld(), Destination, 10.f, 12, FColor(0, 255, 0), false, 100.f);
	}
	DestinationVector = (Destination - PawnAsBot->GetActorLocation()).GetSafeNormal();
	DrawDebugDirectionalArrow(GetWorld(), PawnAsBot->GetActorLocation(), Destination, 30.f, FColor(0, 0, 255), false, 100.f);
	PawnAsBot->AddMovementInput(FVector(DestinationVector.X, DestinationVector.Y, 0.f));
	UE_LOG(LogTemp, Log, TEXT("Sending movement to Actor: %s"), *DestinationVector.GetSafeNormal2D().ToString());

	//UE_LOG(LogTemp, Log, TEXT("Tick?"));
	if (false)
	{
		if (PawnAsBot)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PawnAsBot"));
			if (AActor* Target = PawnAsBot->GetTarget())
			{
				// We do have a target. Shamble toward it and attempt violence!
				FVector DirectionToTarget = (Target->GetActorLocation() - PawnAsBot->GetActorLocation()).GetSafeNormal2D();
				float DistanceToTarget = FVector::Dist2D(Target->GetActorLocation(), PawnAsBot->GetActorLocation());
				//FVector Direction; float Length;
				//DirectionToTarget.ToDirectionAndLength(Direction, Length);
				UE_LOG(LogTemp, Warning, TEXT("%f"), DistanceToTarget);
				float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsBot->GetActorForwardVector());
				float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsBot->GetActorRightVector());
				//Relative angle to add to where we facing to be where we want to face
				float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);

				if (PawnAsBot->BotAIShouldAttack())
				{
					//UE_LOG(LogTemp, Warning, TEXT("We think we should attack"));
					PawnAsBot->AddAttackInput();
				}
				else //delete this 'else' if we want to move when attacking
				{
					//MoveToActor(Target);
					//UE_LOG(LogTemp, Warning, TEXT("We move"));
					//Move faster when facing toward the target so that we turn more accurately/don't orbit.
					PawnAsBot->AddMovementInput(FVector(0.0f, 0.0f, 0.0f), FMath::GetMappedRangeValueClamped(FVector2D(-0.707f, 0.707f), FVector2D(0.0f, 1.0f), DotToTarget));
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
}

ATargetPoint* ABotController::GetRandomWaypoint() const
{
	if (Waypoints.Num() > 0)
	{
		auto index = FMath::RandRange(0, Waypoints.Num() - 1);
		return Cast<ATargetPoint>(Waypoints[index]);
		UE_LOG(LogTemp, Log, TEXT("%d"), index);
	}
	return nullptr;
}

void ABotController::GoToRandomWaypoint()
{
	UE_LOG(LogTemp, Log, TEXT("In GoToRandWP"));
	ATargetPoint* WhereTo = GetRandomWaypoint();
	if (WhereTo != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Sending MoveToActor"));
		FPathFollowingResult Result = MoveToActor(WhereTo);
		UE_LOG(LogTemp, Log, TEXT("Move result: %d"), Result.IsFailure() ? 1 : 0);
	}
}

void ABotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//UE_LOG(LogTemp, Warning, TEXT("Posessed the pawn"));
	PawnAsBot = Cast<ABot>(GetPawn());
}

void ABotController::OnUnPossess()
{
	Super::OnUnPossess();
	PawnAsBot = nullptr;
}

