// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "IKillableInterface.h"
#include "Tank.generated.h"

//Struct to manage inputs
USTRUCT(BlueprintType)
struct FTankInput {
public:
	GENERATED_BODY()

	//What these uints really are is a super nice booleans stored within
	//a bulk so they only take 1 byte each
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bFire1 : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bEsc : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bMoveForward : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bMoveBackward : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bTurnLeft : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bTurnRight : 1;

	void MoveForward(bool bPressed);
	void MoveBackward(bool bPressed);

	void TurnLeft(bool bPressed);
	void TurnRight(bool bPressed);

	void Fire1(bool bPressed);
	void Esc(bool bPressed);
};

UCLASS()
class TANKS_API ATank : public APawn, public IKillableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE const FTankInput& GetCurrentInput() const { return TankInput; }

	//~ Begin interface
	virtual void GetShot() override;
	//~ End interface

	bool IsDead();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input structure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
		FTankInput TankInput;

	//Maximum turn rate (degrees/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
		float YawSpeed;

	//Maximum movement rate (units/second) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
		float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank")
		TSubclassOf<AActor> Projectile;

private:
	//Don't have to be the same as in mappings

	void MoveForwardPressed();
	void MoveForwardReleased();
	void MoveBackwardPressed();
	void MoveBackwardReleased();

	void TurnLeftPressed();
	void TurnLeftReleased();
	void TurnRightPressed();
	void TurnRightReleased();

	void Fire1Pressed();
	void Fire1Released();
	void EscPressed();
	void EscReleased();

	// Helpful debug tool - which way is the tank facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		UArrowComponent* TankDirection;

	// Sprite for the tank body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* TankSprite;

	// Body for collision test
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* TankBody;

	//In-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

	// If this value is greater than the current game time, Fire1 is ignored because it has been fired too recently.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		float Fire1ReadyTime;

	// Time to delay between Fire1 commands.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		float Fire1Cooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		bool bIsKilled;
};
