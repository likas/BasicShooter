// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Tanks.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Bot.generated.h"

class ATank;

UCLASS()
class TANKS_API ABot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Set this Bot's target. The base version of this function will handle updating TargetActor and TargetTank appropriately. Input parameter can be NULL.
	UFUNCTION(BlueprintCallable, Category = "AI")
		void SetTarget(AActor* NewTarget);

	// Return the target Actor. Returning NULL indicates no target.
	UFUNCTION(BlueprintCallable, Category = "AI")
		AActor* GetTarget();

	// Return the target Actor as a Tank, if possible. Returning NULL indicates no target, or that the target is not a Tank.
	UFUNCTION(BlueprintCallable, Category = "AI")
		ATank* GetTargetAsTank();

	// Bots will call this on Tick.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
		void BotAI(float DeltaSeconds);
	virtual void BotAI_Implementation(float DeltaSeconds);

	// This function asks the Bot if it is in position to attack its current target. It does not actually command the Bot to attack.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
		bool BotAIShouldAttack();
	virtual bool BotAIShouldAttack_Implementation();

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

	/* The actor we're targeting. Will be NULL if there is no target. */
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
		AActor* TargetActor;

	/* The actor we're targeting, cast to a Tank and cached. Will be NULL if no target or if the target actor is not a Tank. */
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
		ATank* TargetTank;

	// Current rotation input.
	float YawInput;

	// Current attack button status.
	uint32 bAttackInput : 1;

protected:
	/** Camera effect, if any, to play when the player is hit by this Bot. */
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> HitShake;

	/** Current health value. Might be fun to have different values for different attack types, e.g. fire, explosions, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float Health;

	/** Sight distance (when no target is present) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float SightDistance;

	/** Sight half-angle in degrees (when no target is present) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float SightAngle;

	/** Max yaw speed per second in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float YawSpeed;

	/** Speed when wandering around, or when facing away from target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float WalkSpeed;

	/** Speed when rushing toward target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot", meta = (ClampMin = "0.0"))
		float RunSpeed;

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
};
