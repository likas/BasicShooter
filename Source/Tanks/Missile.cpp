// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "Tanks.h"
#include "PaperSpriteComponent.h"
#include "IKillableInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Wall.h"



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
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Missile spawned"));
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();

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
		This (yet another) workaround to prevent "stucking". "Stucking" is when right after reflection another
		collision occurs because the projectile is still close to the surface it collided
		with in the first place
		*/
		HasCollidedOnPreviousTick = HasCollidedOnPreviousTick && HasCollided ? 1 : 0;
		if (HasCollided && !HasCollidedOnPreviousTick)
		{
			//UE_LOG(LogTemp, Log, TEXT("Bumped into %s"), *OutHit.GetActor()->GetActorLabel());
			HasCollidedOnPreviousTick = true;

			if (IKillableInterface* ItBleeds = Cast<IKillableInterface>(OutHit.GetActor()))
			{
				ItBleeds->GetShot();
				Explode();
			}
			else if (AWall* Wall = Cast<AWall>(OutHit.GetActor()))
			{
				Explode();
			}
			//SetActorEnableCollision(false);

			//Getting a Direction vector
			FVector CurrentForwardVector = MissileSprite->GetForwardVector().GetSafeNormal();

			//For given Direction vector and Normal get back the Reflection vector
			FVector ReflectionVector = FMath::GetReflectionVector(CurrentForwardVector, OutHit.Normal);

			//Getting a NewYaw from the Reflection vector
			float NewYaw = FMath::RadiansToDegrees(FMath::Atan2(ReflectionVector.Y, ReflectionVector.X));

			//Setting a new direction for our bullet
			FRotator NewRotation = FRotator(0.f, NewYaw, 0.f);
			SetActorRotation(NewRotation);
		}
		else
		{
			SetActorLocation(DesiredEndLoc);
		}
	}
}

void AMissile::Explode()
{
	SetActorEnableCollision(false);
	OnExplode();
}

void AMissile::OnExplode_Implementation()
{
	Destroy();
}
