// Fill out your copyright notice in the Description page of Project Settings.

#include "Bot.h"
#include "Tanks.h"
#include "Tank.h"
#include "Missile.h"
#include "Wall.h"
#include "GameFramework/Pawn.h"
#include "Math/Vector.h"
#include "Engine/StaticMeshActor.h"
#include "PaperSpriteComponent.h"
#include <AIController.h>

// Sets default values
ABot::ABot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BotBase"));
	}

	BotDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("BotDirection"));
	BotDirection->SetupAttachment(BotSprite);

	BotSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BotSprite"));
	BotSprite->SetupAttachment(RootComponent);

	BotBody = CreateDefaultSubobject<USphereComponent>(TEXT("BotBody"));
	BotBody->SetupAttachment(RootComponent);
	/* This was a box, but it was constantly stucking. This is a temporary workaroung */
	BotBody->InitSphereRadius(44.f);
	BotBody->SetCollisionProfileName(TEXT("Bot:Move"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	SightDistance = 300.f;
	SightAngle = 180.f;
	YawSpeed = 90.f;
	MoveSpeed = 100.f;
	AttackDistance = 100.f;
	AttackAngle = 30.f;
	AttackCooldown = 0.5f;
}

// Called when the game starts or when spawned
void ABot::BeginPlay()
{
	Super::BeginPlay();

	bIsKilled = false;
	/* 
	Get our player. Bot tracks the player constantly, but don't "see" him.
	It means it will react only if the player is closer than SightDistance
	*/
	AActor* Target = UGameplayStatics::GetPlayerPawn(this, 0);
	SetTarget(Target);

	//Check if target is a tank
	if (ATank* TargetAsTank = Cast<ATank>(Target))
	{
		/* This is fine */
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is NOT a tank"));
	}
}

// Called every frame
void ABot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Respond to controls if we're not dead.
	if (bIsKilled == false)
	{
		if (GetAttackInput())
		{
			//Spawn a missile
			if (UWorld* World = GetWorld()) {
				float CurrentTime = World->GetTimeSeconds();
				if (AttackAvailableTime <= CurrentTime)
				{
					FVector Loc = GetActorLocation() + FVector(BotDirection->GetForwardVector().X * 70.f, BotDirection->GetForwardVector().Y * 70.f, 0.f);
					FRotator Rot = BotDirection->GetComponentRotation();
					if (AActor* NewProjectile = World->SpawnActor(Projectile))
					{
						UE_LOG(LogTemp, Log, TEXT("Projectile spawned by Bot"));
						NewProjectile->SetActorLocation(Loc);
						NewProjectile->SetActorRotation(Rot);
					}

					// Set the cooldown timer.
					AttackAvailableTime = CurrentTime + AttackCooldown;
				}
			}
		}

		float MaxYawThisFrame = YawSpeed * DeltaTime;
		FRotator DesiredRot = GetActorRotation() + FRotator(0.0f, FMath::Clamp(GetRotationInput(), -MaxYawThisFrame, MaxYawThisFrame), 0.0f);
		SetActorRotation(DesiredRot);

		/* Move, if the controller tells us so */
		FVector PendingMovement = GetPendingMovementInputVector(); 
		SetActorLocation(GetActorLocation() + (PendingMovement * DeltaTime * MoveSpeed));

	}

	// Make sure to consume all input on each frame.
	ConsumeMovementInputVector();
	ConsumeRotationInput();
	ConsumeAttackInput();
}

// Called to bind functionality to input
void ABot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool ABot::BotAIShouldAttack_Implementation()
{
	//BotAIShouldPow!
	if (AActor* Target = GetTarget()) {
		UE_LOG(LogTemp, Log, TEXT("I have the target!"));
		//if have a visual
		float DistanceToTarget = FVector::DistXY(Target->GetActorLocation(), GetActorLocation());
		if (DistanceToTarget <= SightDistance)
		{
			FVector OurLocation = GetActorLocation();
			FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation).GetSafeNormal2D();
			float DotToTarget = FVector::DotProduct(DirectionToTarget, GetActorForwardVector());

			//Target's within an attack angle?
			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
			{
				UE_LOG(LogTemp, Log, TEXT("Have visual on the target!"));
				//Check if the shot is clear
				FHitResult OutHit;
				FCollisionShape CollisionShape;
				CollisionShape.SetCapsule(10.f, 8.f);
				FVector ShotLandLocation = BotDirection->GetComponentLocation() + FVector(GetActorForwardVector().X * DistanceToTarget, GetActorForwardVector().Y * DistanceToTarget, 0.f);
				if (GetWorld()->SweepSingleByProfile(OutHit, BotDirection->GetComponentLocation(), ShotLandLocation, FQuat::Identity, "Bullet", CollisionShape))
				{
					
					// Very useful debug thing, only draw if PIE
					if (GetWorld()->IsPlayInEditor())
					{
						DrawDebugDirectionalArrow(GetWorld(), BotDirection->GetComponentLocation(), ShotLandLocation, 30.f, FColor(255, 0, 0), false, 100.f);
					}
					
					if (ATank* HitActor = Cast<ATank>(OutHit.GetActor()))
					{
						UE_LOG(LogTemp, Log, TEXT("Hit that bastard!"));
					}
					else if (ABot* Ourself = Cast<ABot>(OutHit.GetActor()))
					{
						UE_LOG(LogTemp, Warning, TEXT("I shot my leg! Bwaaa!!"));
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("Something is blocking the shot"));
					}

					return true;
				}
			}
		}
	}
	return false;
}

bool ABot::BotAITargetInSight_Implementation() {
	float DistanceToTarget = FVector::DistXY(GetTarget()->GetActorLocation(), GetActorLocation());
	if (DistanceToTarget <= SightDistance)
	{
		return true;
	}
	else return false;
}

bool  ABot::BotAIObstacleInTheWay_Implementation(FVector& NormalToObstacle)
{
	//Handle collisions
	if (UWorld* World = GetWorld())
	{
		TArray<FHitResult> HitResults;
		//FVector BoxSize = BotBody->GetScaledBoxExtent();
		float BoxSize = BotBody->GetScaledSphereRadius();
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(BoxSize);
		FVector GunEnd = GetActorLocation() + FVector(GetActorForwardVector().X * 20.f, GetActorForwardVector().Y * 20.f, 0.f);

		//Spawn a bullet on gun end (TODO Add a component for this)
		if (World->SweepMultiByProfile(HitResults, GetActorLocation(), GunEnd, BotBody->GetComponentRotation().Quaternion(), "Bot:Move", CollisionShape))
		{
			for (const FHitResult& HitResult : HitResults)
			{
				/*
				TODO Delete first two ifs and check if it brakes something
				*/
				if (ABot* Bot = Cast<ABot>(HitResult.GetActor())) {
					//UE_LOG(LogTemp, Log, TEXT("Bot: Hit by Bot (itself"))
					//Dont move
				}
				else if (AMissile* Bullet = Cast<AMissile>(HitResult.GetActor()))
				{
					//UE_LOG(LogTemp, Log, TEXT("Bot: Hit by Missile"))
				}
				else if (AStaticMeshActor* Wall = Cast<AStaticMeshActor>(HitResult.GetActor()))
				{
					/* 
					This is to get out if we're already started penetrating through
					(adds a little nudge in the direction of the normal of what we hit)
					*/
					SetActorLocation(GetActorLocation() + FVector(HitResult.Normal.X * HitResult.PenetrationDepth, HitResult.Normal.Y * HitResult.PenetrationDepth, 0.f));
					NormalToObstacle = HitResult.Normal;
					//UE_LOG(LogTemp, Warning, TEXT("Bot: Hit by Wall or Obstacle"))
				}
				//UE_LOG(LogTemp, Log, TEXT("Bot: Hit by: %s"), *HitResult.GetActor()->GetName())
			}
			return true;
		}
	}
	NormalToObstacle = FVector::ZeroVector;
	return false;
}

void ABot::GetShot()
{
	bIsKilled = true;
	UE_LOG(LogTemp, Log, TEXT("Bot is killed!"));
}

bool ABot::IsDead() 
{
	return bIsKilled;
}

/* Target management */

void ABot::SetTarget(AActor* NewTarget)
{
	TargetActor = NewTarget;
}

AActor* ABot::GetTarget()
{
	return TargetActor;
}

//~
//~ New Bot Input
//~

void ABot::AddRotationInput(float DeltaYawDegrees)
{
	YawInput += DeltaYawDegrees;
}

float ABot::GetRotationInput()
{
	return YawInput;
}

float ABot::ConsumeRotationInput()
{
	float RetVal = YawInput;
	YawInput = 0.0f;
	return RetVal;
}

void ABot::AddAttackInput()
{
	bAttackInput = true;
}

bool ABot::GetAttackInput()
{
	return bAttackInput;
}

bool ABot::ConsumeAttackInput()
{
	bool bRetVal = bAttackInput;
	bAttackInput = false;
	return bRetVal;
}