// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Tanks.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "IKillableInterface.h"
#include "Bot.generated.h"

class ATank;

UCLASS()
class TANKS_API ABot : public APawn, public IKillableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Set this Bot's target. The base version of this function will handle updating TargetActor appropriately. Input parameter can be NULL.
	UFUNCTION(BlueprintCallable, Category = "AI")
		void SetTarget(AActor* NewTarget);

	// Return the target Actor. Returning NULL indicates no target.
	UFUNCTION(BlueprintCallable, Category = "AI")
		AActor* GetTarget();

	// Return the target Actor as a Tank, if possible. Returning NULL indicates no target, or that the target is not a Tank.
	UFUNCTION(BlueprintCallable, Category = "AI")
		bool IsDead();

	// This function asks the Bot if there's an obstacle blocking the way.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
		bool BotAIObstacleInTheWay(FVector& NormalToObstacle);
	virtual bool  BotAIObstacleInTheWay_Implementation(FVector& NormalToObstacle);

	// This function asks the Bot if it is in position to attack its current target. It does not actually command the Bot to attack.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
		bool BotAIShouldAttack();
	virtual bool BotAIShouldAttack_Implementation();

	// This function asks the Bot if it sees its current target.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
		bool BotAITargetInSight();
	virtual bool BotAITargetInSight_Implementation();

	//~ Begin interface
	virtual void GetShot() override;
	//~ End interface

	//~
	//~ New Bot Input
	//~

	/** Add rotation equal to the yaw (in degrees) provided. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
		virtual void AddRotationInput(float DeltaYawDegrees);

	/** Get rotation input. Returns pre-clear value. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
		virtual float GetRotationInput();

	/** Get (and clear) rotation input. Returns pre-clear value. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
		virtual float ConsumeRotationInput();

	/** Press the attack button. The pawn will know what to do with this. No arguments because it's a bool and this sets it to true. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
		virtual void AddAttackInput();

	/** Get the status of the attack button. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
		virtual bool GetAttackInput();

	/** Get (and clear) the status of the attack button. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
		virtual bool ConsumeAttackInput();

private:
	//Facing direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bot", meta = (AllowPrivateAccess = "true"))
		UArrowComponent* BotDirection;

	// Sprite for the body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bot", meta = (AllowPrivateAccess = "true"))
		class UPaperSpriteComponent* BotSprite;

	// Body for collision test
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bot", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* BotBody;

	/* The actor we're targeting. Will be NULL if there is no target. */
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
		AActor* TargetActor;

	// Current rotation input.
	float YawInput;

	// Current attack button status.
	uint32 bAttackInput : 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/* We shoot with this. Don't forget to set it in blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot")
		TSubclassOf<AActor> Projectile;

	/** Sight distance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float SightDistance;

	/** Sight half-angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float SightAngle;

	/** Max yaw speed per second in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float YawSpeed;

	/** Speed when wandering around */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float WalkSpeed;

	/** Attack distance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float AttackDistance;

	/** Half-angle (in degrees) for our attack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float AttackAngle;

	/** Minimum time between attacks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float AttackCooldown;

	/** Game time, in seconds, when the Bot will be allowed to attack again. */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Bot")
		float AttackAvailableTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
		bool bIsKilled;
};
