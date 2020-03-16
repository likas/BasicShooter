// Fill out your copyright notice in the Description page of Project Settings.

#include "BotController.h"
#include "Tanks.h"
#include "Bot.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Engine/TargetPoint.h"


void ABotController::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("BeginPlay1"));
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BeginPlay"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Waypoints);
	UE_LOG(LogTemp, Warning, TEXT("%d WPs"), Waypoints.Num());
	UE_LOG(LogTemp, Log, TEXT("Got the waypoints"));
	GoToRandomWaypoint();
	UE_LOG(LogTemp, Log, TEXT("Finished?"));
}

void ABotController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	//NavigationHandle::PointCheck()
	//MoveToLocation(FVector());
	GoToRandomWaypoint();
}



/*ABotController::ABotController(const FPostConstructInitializeProperties& PCIP)
{
	Super(PCIP.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
}*/

void ABotController::Tick(float DeltaTime)
{
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
				FVector Direction; float Length;
				DirectionToTarget.ToDirectionAndLength(Direction, Length);
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
					MoveToActor(Target);
					//UE_LOG(LogTemp, Warning, TEXT("We move"));
					// Move faster when facing toward the target so that we turn more accurately/don't orbit.
					//PawnAsBot->AddMovementInput(FVector(0.0f, 0.0f, 0.0f), FMath::GetMappedRangeValueClamped(FVector2D(-0.707f, 0.707f), FVector2D(0.0f, 1.0f), DotToTarget));
					// Attempt the entire turn in one frame. The Zombie itself will cap this, we're only expressing our desired turn amount here.
					//PawnAsBot->AddRotationInput(DeltaYawDesired);
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("NoTarget"));
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

