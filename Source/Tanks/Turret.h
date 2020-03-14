// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tanks.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Runtime\Engine\Classes\Components\ArrowComponent.h>
#include "Turret.generated.h"

UCLASS()
class TANKS_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Max turn rate in degrees/second for the turret.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
		float YawSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
		class ATank* Tank;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret")
		TSubclassOf<AActor> Projectile;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** The name of the socket at the muzzle - used for spawning missiles. */
	static const FName MuzzleSocketName;

private:
	// Helpful debug tool - which way is the turret facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
		UArrowComponent* TurretDirection;

	// Sprite for the turret.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* TurretSprite;

	FORCEINLINE UArrowComponent* GetTurretDirection() { return TurretDirection; }

	// Time to delay between Fire1 commands.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
		float Fire1Cooldown;

	// If this value is greater than the current game time, Fire1 is ignored because it has been fired too recently.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
		float Fire1ReadyTime;
};
