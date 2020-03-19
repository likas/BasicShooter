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

		auto randPoint = NavSystem->GetRandomPointInNavigableRadius(GWorld, PawnAsBot->GetActorLocation(),
			FMath::RandRange(100, 1000));

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
				DrawDebugSphere(GetWorld(), TravelPath->PathPoints[i], 10.f, 12, FColor(255, 0, 0), false, 100.f);
				TravelPoints.Enqueue(TravelPath->PathPoints[i]);
				//UE_LOG(LogTemp, Warning, TEXT("%d. Added Random Location: %f %f %f"), i, TravelPath->PathPoints[i].X, TravelPath->PathPoints[i].Y, TravelPath->PathPoints[i].Z);
				UE_LOG(LogTemp, Warning, TEXT("%d. Added Random Location: %s"), i, *TravelPath->PathPoints[i].ToString());
			}
			
		}
	}
	else
	{
		return Return;
	}
	bool bDequeueSuccessful = TravelPoints.Dequeue(Return);
	if(bDequeueSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Dequeue successful"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Dequeue failed"));
	}
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
	UE_LOG(LogTemp, Log, TEXT("We tick"));
	FVector NormalToObstacle;
	bool bBlockedByObstacle = PawnAsBot->BotAIObstacleInTheWay(NormalToObstacle);
	if(bBlockedByObstacle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Obstacle!"));
	}
	//If we "see player"
	if (PawnAsBot->BotAITargetInSight())
	//if(false)
	{
		Destination = GetNewMovePoint();
		//If the trajectory of our sight is intercepting with a player's body sprite
		if (PawnAsBot->BotAIShouldAttack()) 
		{
			PawnAsBot->AddAttackInput();
		}
		
		//get angle to target
		//FVector DestinationVector = PawnAsBot->GetTarget()->GetActorLocation() - PawnAsBot->GetActorLocation();

		FVector DestinationVector;
		

		DestinationVector = PawnAsBot->GetActorLocation() - PawnAsBot->GetTarget()->GetActorLocation();
		
		bBlockedByObstacle = PawnAsBot->BotAIObstacleInTheWay(NormalToObstacle);
		if (bBlockedByObstacle)
		{
			DestinationVector = NormalToObstacle.RightVector;
		}
		float DotToTarget = FVector::DotProduct(DestinationVector.GetSafeNormal(), PawnAsBot->GetActorForwardVector());
		//float SidewaysDotToTarget = FVector::DotProduct(DestinationVector.GetSafeNormal(), PawnAsBot->GetActorRightVector().RotateAngleAxis(180.f, PawnAsBot->GetTransform().GetUnitAxis(EAxis::X)));
		float SidewaysDotToTarget = FVector::DotProduct(DestinationVector.GetSafeNormal(), PawnAsBot->GetActorRightVector());
		float Angle = FMath::RadiansToDegrees( FMath::Acos(DotToTarget) );

		UE_LOG(LogTemp, Log, TEXT("Angle to target is: %f"), Angle);
		

		PawnAsBot->AddRotationInput(SidewaysDotToTarget * 10);
		if (!bBlockedByObstacle) {
			PawnAsBot->AddMovementInput(FVector(DestinationVector.GetSafeNormal().X, DestinationVector.GetSafeNormal().Y, 0.f));
		}

		if (Angle < 40)
		{
			//rotate so it's enough to shoot
		}
		else if (Angle > 40 && Angle < 90) {
			//
		}
		else
		{
			//flee backwards
		}

	}
	else 
	{
		UE_LOG(LogTemp, Log, TEXT("Distance between Dest and Loc: %f"), FVector::DistXY(PawnAsBot->GetActorLocation(), Destination));
		//Give movement input
		FVector DestinationVector;
		FVector Normal;
		if (FVector::DistXY(PawnAsBot->GetActorLocation(), Destination) <= 3.f || PawnAsBot->BotAIObstacleInTheWay(Normal)) {
			Destination = GetNewMovePoint();

			//DrawDebugSphere(GetWorld(), Destination, 10.f, 12, FColor(0, 255, 0), false, 100.f);
		}
		DestinationVector = (Destination - PawnAsBot->GetActorLocation()).GetSafeNormal();
		DrawDebugDirectionalArrow(GetWorld(), PawnAsBot->GetActorLocation(), Destination, 30.f, FColor(0, 0, 255), false, 100.f);
		//PawnAsBot->AddMovementInput(FVector(DestinationVector.X, DestinationVector.Y, 0.f));
		//UE_LOG(LogTemp, Log, TEXT("Sending movement to Actor: %s"), *DestinationVector.GetSafeNormal2D().ToString());


		float DotToTarget = FVector::DotProduct(DestinationVector, PawnAsBot->GetActorForwardVector());
		float SidewaysDotToTarget = FVector::DotProduct(DestinationVector, PawnAsBot->GetActorRightVector());
		UE_LOG(LogTemp, Log, TEXT("DotProduct is: %f"), DotToTarget);
		UE_LOG(LogTemp, Log, TEXT("Sideways to dot is: %f"), SidewaysDotToTarget);

		float Angle = PawnAsBot->GetActorForwardVector().Rotation().Yaw - DestinationVector.Rotation().Yaw;
		float Acos = FMath::RadiansToDegrees( FMath::Acos(DotToTarget) );
		UE_LOG(LogTemp, Warning, TEXT("Angle and Acos is: %f, %f"), Angle, Acos);
	
		if (FMath::Sign(DotToTarget) != FMath::Sign(SidewaysDotToTarget) && FMath::IsNearlyZero(DotToTarget) && FMath::IsNearlyZero(SidewaysDotToTarget)) { SidewaysDotToTarget += 0.5f; }
		PawnAsBot->AddRotationInput((SidewaysDotToTarget) * 10);

		if (SidewaysDotToTarget <= 0.1)
		{
			if (!bBlockedByObstacle) {
				PawnAsBot->AddMovementInput(FVector(DestinationVector.X, DestinationVector.Y, 0.f));
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

