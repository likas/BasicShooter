// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "PaperSpriteComponent.h"
#include <Runtime\Engine\Classes\Components\ArrowComponent.h>
#include "Camera/CameraComponent.h"
#include "Tank.generated.h"

//Struct to manage inputs
USTRUCT(BlueprintType)
struct FTankInput {
public:
	GENERATED_BODY()
		//Sanitized movement input, ready to be used by game logic
		UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		FVector2D MovementInput;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bFire1 : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bFire2 : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bMoveForward : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bMoveBackward : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bTurnLeft : 1;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Tank Input")
		uint32 bTurnRight : 1;
	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);

	void MoveForward(bool bPressed);
	void MoveBackward(bool bPressed);

	void TurnLeft(bool bPressed);
	void TurnRight(bool bPressed);

	void Fire1(bool bPressed);
	void Fire2(bool bPressed);



private:
	//Internal raw data
	FVector2D RawMovementInput;
};

UCLASS()
class TANKS_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input structure
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
		FTankInput TankInput;

	//Maximum turn rate (degrees/seconds) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
		float YawSpeed;

	//Maximum movement rate (sentimeters/seconds) of the tank
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
		float MoveSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE const FTankInput& GetCurrentInput() const { return TankInput; }

private:
	//Dont have to be the same as in mappings
	void MoveX(float AxisValue); 
	void MoveY(float AxisValue);

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
	void Fire2Pressed();
	void Fire2Released();

	// Helpful debug tool - which way is the tank facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* TankDirection;

	// Sprite for the tank body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* TankSprite;

	//The actor used as the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ChildTurret;

	//In-game camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;
};
