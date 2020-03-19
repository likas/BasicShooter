// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "Tanks.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "IKillableInterface.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>
#include "Math/UnrealMathUtility.h"
//#include <Runtime\Core\Private\Math\UnrealMath.cpp>
//#include <Runtime/Core/Public/Math/UnrealMathUtility.h>
//#include <Runtime/Core/Public/Math/UnrealMath.h>



// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MissileBase"));
	MissileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MissileSprite"));
	MissileSprite->SetupAttachment(RootComponent);

	MovementCollisionProfile = "Bullet";

	HasCollidedOnPreviousTick = false;

	Speed = 200.f;
	Radius = 20.f;
	//Radius = 3.f;
	Lifespan = 10.f;
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Missile message"));
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AMissile::Explode, Lifespan);
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	//FVector unknown = GetTransform().GetUnitAxis(EAxis::X);

	//UE_LOG(LogTemp, Log, TEXT("Unknown (%f %f %f)"), unknown.X, unknown.Y, unknown.Z);
	FVector DesiredEndLoc = Loc + ((DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X));
	SetActorLocation(DesiredEndLoc);
	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 8.f);
		//if we hit something
		bool HasCollided = false;
		HasCollided = World->SweepSingleByProfile(OutHit, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape);
		/*
		This is to prevent "stucking". "Stucking" is when right after reflection another
		collision occurs because the projectile is still close to the surface it collided
		with in the first place
		*/
		HasCollidedOnPreviousTick = HasCollidedOnPreviousTick && HasCollided ? 1 : 0;
		/*UE_LOG(LogTemp, Log, TEXT("Ret = %x"), Ret);*/
		if (HasCollided && !HasCollidedOnPreviousTick)
		{
			//UE_LOG(LogTemp, Log, TEXT("Bumped into %s"), *OutHit.GetActor()->GetActorLabel());
			HasCollidedOnPreviousTick = true;

			if(IKillableInterface* ItBleeds = Cast<IKillableInterface>(OutHit.GetActor()))
			{
				ItBleeds->GetShot();
				Explode();
			}

			SetActorEnableCollision(false);
			//OutHit.Actor.Get()->SetActorEnableCollision(false);
			//Getting a Direction vector
			FVector CurrentForwardVector = MissileSprite->GetForwardVector().GetSafeNormal();
			//For given Direction vector and Normal get back the Reflection vector
			FVector ReflectionVector = FMath::GetReflectionVector(CurrentForwardVector, OutHit.Normal);
			//Getting a NewYaw from the Reflection vector
			float NewYaw = FMath::RadiansToDegrees(FMath::Atan2(ReflectionVector.Y, ReflectionVector.X));
			//Setting a new direction for our bullet
			FRotator NewRotation = FRotator(0.f, NewYaw, 0.f);
			SetActorRotation(NewRotation);


			/*SetActorLocation(OutHit.Location);
			if (IDamageInterface* DamageActor = Cast<IDamageInterface>(OutHit.Actor.Get()))
			{
				DamageActor->ReceiveDamage(DirectDamage);
			}*/
			//AActor* DamageActor = Cast<AActor>(OutHit.Actor.Get());
			//SetActorLocation(OutHit.Location);
			//FVector CurrentForwardVector = MissileSprite->GetForwardVector().GetSafeNormal();
			//FVector ReflectionVector = FMath::GetReflectionVector(CurrentForwardVector, OutHit.Normal);
			//Getting a Yaw from vector
			/*float NewYaw = FMath::RadiansToDegrees(FMath::Atan2(CurrentForwardVector.Y, CurrentForwardVector.X));
			//UKismetMathLibrary::GetYawPitchFromVector(CurrentForwardVector, NewYaw, NewPitch);
			UE_LOG(LogTemp, Log, TEXT("CurrentYaw = %f"), NewYaw);
			NewYaw = 360 - NewYaw;
			UE_LOG(LogTemp, Log, TEXT("AdjustedCurrentYaw = %f"), NewYaw);
			FRotator NewRotation = FRotator(0.f, NewYaw, 0.f);
			FQuat QuatRotation = FQuat(NewRotation);
			AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
			UE_LOG(LogTemp, Log, TEXT("ResultingYaw = %f"), GetActorRotation().Yaw);

			/*if (World->SweepSingleByProfile(OutHit, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape)) 
			{
				Explode();
			}*/
			/*float OldYaw = FMath::RadiansToDegrees(FMath::Atan2(CurrentForwardVector.Y, CurrentForwardVector.X));
			float NewYaw = FMath::RadiansToDegrees(FMath::Atan2(ReflectionVector.Y, ReflectionVector.X));
			UE_LOG(LogTemp, Log, TEXT("OldYaw = %f\nReflectionYaw = %f"), OldYaw, NewYaw);
			FRotator NewRotation = FRotator(0.f, NewYaw, 0.f);
			FQuat QuatRotation = FQuat(NewRotation);
			SetActorRotation(NewRotation);*/
			//AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
			//DesiredEndLoc = ReflectionVector + ((DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X));
			//SetActorLocation(DesiredEndLoc);
			//SetActorRelativeLocation(DesiredEndLoc);
		}
		else
		{
			SetActorLocation(DesiredEndLoc);
		}
	}
}


void AMissile::Explode()
{
	GetWorldTimerManager().ClearTimer(ExplodeTimerHandle);
	SetActorEnableCollision(false);
	OnExplode();
}

void AMissile::OnExplode_Implementation()
{
	Destroy();
}
