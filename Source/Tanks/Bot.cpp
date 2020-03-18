// Fill out your copyright notice in the Description page of Project Settings.

#include "Bot.h"
#include "Tanks.h"
#include "Tank.h"
#include "GameFramework/Pawn.h"
#include "Math/Vector.h"
#include "Missile.h"
#include "PaperSpriteComponent.h"

const FName ABot::MuzzleSocketName(TEXT("GunBarrel"));

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

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Health = 100.f;
	SightDistance = 300.f;
	SightAngle = 180.f;
	YawSpeed = 90.f;
	WalkSpeed = 100.f;
	RunSpeed = 45.f;
	AttackDistance = 100.f;
	AttackAngle = 30.f;
	AttackCooldown = 1.f;
}

// Called when the game starts or when spawned
void ABot::BeginPlay()
{
	Super::BeginPlay();
	AActor* Target = UGameplayStatics::GetPlayerPawn(this, 0); SetTarget(Target);
	//Check if target is a tank
	if (ATank* TargetAsTank = Cast<ATank>(Target)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is A TANK"));
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

	//BotAI(DeltaTime);

	BotAIShouldAttack();

	FVector PendingMovement = GetPendingMovementInputVector();
	UE_LOG(LogTemp, Log, TEXT("Movement in Actor: %s"), *PendingMovement.ToString());
	SetActorLocation(GetActorLocation() + (PendingMovement * DeltaTime * WalkSpeed));
	FRotator DesiredRot = GetActorRotation() + FRotator(0.0f, GetRotationInput(), 0.0f);
	SetActorRotation(DesiredRot);
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

void ABot::BotAI_Implementation(float DeltaSeconds)
{

	if (AActor* Target = GetTarget()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Got a target"));
		float DistanceToTarget = FVector::DistXY(GetActorLocation(), Target->GetActorLocation());
		if(DistanceToTarget < SightDistance)
		{
			//Try to get away
			//Send a signal that we want to get away

		}
	}

	UE_LOG(LogTemp, Warning, TEXT("TheBot!"));
	// The Bot always moves unless attacking. If moving, it moves between WalkSpeed and RunSpeed.
	FVector DesiredMovement = GetAttackInput() ? FVector::ZeroVector : (FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(WalkSpeed, RunSpeed), GetPendingMovementInputVector().X)) * DeltaSeconds * GetActorForwardVector();
	FVector OriginalLocation = GetActorLocation();
	FVector DesiredLoc = OriginalLocation + DesiredMovement;
	float MaxYawThisFrame = YawSpeed * DeltaSeconds;
	FRotator DesiredRot = GetActorRotation() + FRotator(0.0f, FMath::Clamp(GetRotationInput(), -MaxYawThisFrame, MaxYawThisFrame), 0.0f);

	SetActorLocationAndRotation(DesiredLoc, DesiredRot.Quaternion(), true);
	FVector DistanceWalked = GetActorLocation() - OriginalLocation;
	if (!DistanceWalked.IsNearlyZero())
	{

	}

	// See if we have a target and deal with it if we do. Find a target if we don't.
	if (AActor* Target = GetTarget())
	{
		// We do have a target. See if we're attacking, since our attacks always hit.
		FVector OurLocation = GetActorLocation();
		FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation).GetSafeNormal2D();
		float DotToTarget = FVector::DotProduct(DirectionToTarget, GetActorForwardVector());

		// Check to see if we should attack based on button status and attack cooldown.
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (GetAttackInput() && (AttackAvailableTime <= CurrentTime))
		{
			AttackAvailableTime = CurrentTime + AttackCooldown;

			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
			{
				float DistSqXY = FVector::DistSquaredXY(Target->GetActorLocation(), OurLocation);
				if (DistSqXY <= (AttackDistance * AttackDistance))
				{
					if (ATank* TankTarget = GetTargetAsTank())
					{
						//TankTarget->DamageHealth(10.0f);
						UE_LOG(LogTemp, Warning, TEXT("Attack!"));
						if (APlayerController* PC = Cast<APlayerController>(TankTarget->GetController()))
						{
							PC->ClientPlayCameraShake(HitShake, 1.0f);
						}
					}
					else
					{
						SetTarget(nullptr);
					}
				}
			}
		}
	}
	else
	{
		// Look for a target. We might not do this every single frame, but for now it's OK.
		// TODO: Make this use a list of registered targets so we can handle multiplayer or add decoys.
		Target = UGameplayStatics::GetPlayerPawn(this, 0); SetTarget(Target);
		/*float DistSqXY = FVector::DistSquaredXY(Target->GetActorLocation(), GetActorLocation());
		if (DistSqXY <= (SightDistance * SightDistance))
		{
			FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			if (FVector::DotProduct(DirectionToTarget, GetActorForwardVector()) >= FMath::Cos(FMath::DegreesToRadians(SightAngle)))
			{
				SetTarget(Target);
			}
		}*/
	}
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
			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
			{
				UE_LOG(LogTemp, Log, TEXT("Have visual on the target!"));
				//Check if the shot is clear
				FHitResult OutHit;
				FCollisionShape CollisionShape;
				CollisionShape.SetCapsule(10.f, 8.f);
				//if (GetWorld()->SweepSingleByProfile(OutHit, BotSprite->GetSocketLocation(MuzzleSocketName), Target->GetActorLocation(), FQuat::Identity, "Bullet", CollisionShape))
				if (GetWorld()->SweepSingleByProfile(OutHit, BotDirection->GetComponentLocation(), BotDirection->GetComponentLocation() + FVector(GetActorForwardVector().X * DistanceToTarget, GetActorForwardVector().Y * DistanceToTarget, 0.f), FQuat::Identity, "Bullet", CollisionShape))
				{
					DrawDebugDirectionalArrow(GetWorld(), BotDirection->GetComponentLocation(), BotDirection->GetComponentLocation() + FVector(GetActorForwardVector().X * DistanceToTarget, GetActorForwardVector().Y * DistanceToTarget, 0.f), 30.f, FColor(255, 0, 0), false, 100.f);
					if (ATank* HitActor = Cast<ATank>(OutHit.GetActor()))
					{
						UE_LOG(LogTemp, Warning, TEXT("Hit that bastard!"));
					}
					else if (ABot* Ourself = Cast<ABot>(OutHit.GetActor()))
					{
						UE_LOG(LogTemp, Warning, TEXT("I shot my leg! Bwaaa!!"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Something is blocking the shot"));
					}
				}
			}
		}
	}
	return true;
}

bool ABot::BotAITargetInSight_Implementation() {
	float DistanceToTarget = FVector::DistXY(GetTarget()->GetActorLocation(), GetActorLocation());
	if (DistanceToTarget <= SightDistance)
	{
		return true;
	}
	else return false;
}

void ABot::SetTarget(AActor* NewTarget)
{
	TargetActor = NewTarget;
	TargetTank = Cast<ATank>(NewTarget);
}

AActor* ABot::GetTarget()
{
	return TargetActor;
}

ATank* ABot::GetTargetAsTank()
{
	return TargetTank;
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