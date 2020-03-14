// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class TANKS_API AMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissile();

protected:
	void Explode();

	FTimerHandle ExplodeTimerHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** What to do when the projectile explodes. The base version just destroys the projectile. */
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile")
		void OnExplode();
	virtual void OnExplode_Implementation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float Speed;
	/** This missile's radius for collisions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		float Radius;

	/** Describes what this missile hits. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		FName MovementCollisionProfile;

private:
	// Sprite for the Missile.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Missile", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* MissileSprite;
};
