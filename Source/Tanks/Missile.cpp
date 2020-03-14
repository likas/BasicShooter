// Fill out your copyright notice in the Description page of Project Settings.

#include "Tanks.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "Missile.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MissileBase"));
	MissileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MissileSprite"));
	MissileSprite->SetupAttachment(RootComponent);

	MovementCollisionProfile = "Bullet";

	Speed = 200.f;
	Radius = 20.f;
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
	FVector DesiredEndLoc = Loc + ((DeltaTime * Speed) * GetTransform().GetUnitAxis(EAxis::X));
	SetActorLocation(DesiredEndLoc);
	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 70.f);
		//if we hit something
		bool Ret = false;
		Ret = World->SweepSingleByProfile(OutHit, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape);
		UE_LOG(LogTemp, Log, TEXT("Ret = %x"), Ret);
		if (Ret)
		{
			/*SetActorLocation(OutHit.Location);
			if (IDamageInterface* DamageActor = Cast<IDamageInterface>(OutHit.Actor.Get()))
			{
				DamageActor->ReceiveDamage(DirectDamage);
			}*/
			AActor* DamageActor = Cast<AActor>(OutHit.Actor.Get());
			SetActorLocation(OutHit.Location);
			FVector CurrentForwardVector = MissileSprite->GetForwardVector();
			float cYaw, cPitch;
			UKismetMathLibrary::GetYawPitchFromVector(CurrentForwardVector, cYaw, cPitch);
			FRotator NewRotation = FRotator(0.f, (cYaw * 2), 0.f);
			FQuat QuatRotation = FQuat(NewRotation);
			AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
			/*if (World->SweepSingleByProfile(OutHit, Loc, DesiredEndLoc, FQuat::Identity, MovementCollisionProfile, CollisionShape)) 
			{
				Explode();
			}*/
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
