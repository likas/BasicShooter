// Fill out your copyright notice in the Description page of Project Settings.

#include "Tanks.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "Missile.h"

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MissileBase"));
	MissileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MissileSprite"));
	MissileSprite->SetupAttachment(RootComponent);

	Speed = 200.f;
	Radius = 20.f;
	
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AMissile::Explode, 1.0f);
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	FVector DesiredEndLoc = Loc + ((DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X));

	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 70.f);
		if (World->SweepSingleByProfile(OutHit, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			/*SetActorLocation(OutHit.Location);
			if (IDamageInterface* DamageActor = Cast<IDamageInterface>(OutHit.Actor.Get()))
			{
				DamageActor->ReceiveDamage(DirectDamage);
			}*/
			Explode();
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
