// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "Tanks.h"
#include "Components/ArrowComponent.h"
//#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "PaperSpriteComponent.h"

void FTankInput::Sanitize() {
	MovementInput = RawMovementInput.ClampAxes(-1.f, 1.f);
	MovementInput = MovementInput.GetSafeNormal();
	RawMovementInput.Set(0.f, 0.f);
}

void FTankInput::MoveX(float AxisValue) {
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue) {
	RawMovementInput.Y += AxisValue;
}

//Moving straight
void FTankInput::MoveForward(bool bPressed)
{
	bMoveForward = bPressed;
}

void FTankInput::MoveBackward(bool bPressed)
{
	bMoveBackward = bPressed;
}

//Moving around
void FTankInput::TurnLeft(bool bPressed)
{
	bTurnLeft = bPressed;
}

void FTankInput::TurnRight(bool bPressed)
{
	bTurnRight = bPressed;
}

//Attacking
void FTankInput::Fire1(bool bPressed)
{
	bFire1 = bPressed;
}

void FTankInput::Fire2(bool bPressed)
{
	bFire2 = bPressed;
}


// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TankBase"));
	}

	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->SetupAttachment(RootComponent);

	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->SetupAttachment(TankDirection);

	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(0.f, 0.f, 0.f)); // Fixed on creation

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.f;
	CameraComponent->AspectRatio = 3.f / 4.f;
	CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(0.f, 0.f, 0.f)); // Fixed on creation

	ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("Turret"));
	ChildTurret->SetupAttachment(TankDirection);

	MoveSpeed = 100.0f;
	//MoveAccel = 200.0f;
	YawSpeed = 5.0f;
	Fire1Cooldown = 0.5f;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TankInput.Sanitize();
	const FTankInput& CurrentInput = GetCurrentInput();
	if (CurrentInput.bMoveForward || CurrentInput.bMoveBackward)
	{
		//UE_LOG(LogTemp, Log, TEXT("Pressed"));
		//ѕолучить вектор направлени€
		FVector WhereToMove = TankDirection->GetForwardVector() * (CurrentInput.bMoveForward ? 1 : -1);
		/*UE_LOG(LogTemp, Log, TEXT("Direction: (%f, %f)"), TankDirection->GetForwardVector().X, TankDirection->GetForwardVector().Y);
		UE_LOG(LogTemp, Log, TEXT("Location: (%f, %f)"), WhereToMove.X, WhereToMove.Y);*/
		//ƒобавить вектор к текущему местоположению
		FVector Pos = GetActorLocation();
		SetActorLocation(Pos + WhereToMove * MoveSpeed * DeltaTime);
	}
	if (CurrentInput.bTurnLeft || CurrentInput.bTurnRight)
	{
		FRotator NewRotation = FRotator(0.f, CurrentInput.bTurnRight ? YawSpeed : -1.f * YawSpeed, 0.f);
		FQuat QuatRotation = FQuat(NewRotation);
		TankDirection->AddLocalRotation(QuatRotation, false, 0, ETeleportType::None);
	}

	if (UWorld* World = GetWorld()) {
		float CurrentTime = World->GetTimeSeconds();
		if (CurrentInput.bFire1 && Fire1ReadyTime <= CurrentTime)
		{
			FVector Loc = GetActorLocation();
			FRotator Rot = TankDirection->GetComponentRotation();
			if (AActor* NewProjectile = World->SpawnActor(Projectile))
			{
				UE_LOG(LogTemp, Log, TEXT("Projectile spawned"));
				NewProjectile->SetActorLocation(Loc);
				NewProjectile->SetActorRotation(Rot);
			}

			// Set the cooldown timer.
			Fire1ReadyTime = CurrentTime + Fire1Cooldown;
		}
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveX", this, &ATank::MoveX); 
	PlayerInputComponent->BindAxis("MoveY", this, &ATank::MoveY);
	PlayerInputComponent->BindAction("MoveForward", EInputEvent::IE_Pressed, this, &ATank::MoveForwardPressed);
	PlayerInputComponent->BindAction("MoveForward", EInputEvent::IE_Released, this, &ATank::MoveForwardReleased);
	PlayerInputComponent->BindAction("MoveBackward", EInputEvent::IE_Pressed, this, &ATank::MoveBackwardPressed);
	PlayerInputComponent->BindAction("MoveBackward", EInputEvent::IE_Released, this, &ATank::MoveBackwardReleased);
	PlayerInputComponent->BindAction("TurnRight", EInputEvent::IE_Pressed, this, &ATank::TurnRightPressed);
	PlayerInputComponent->BindAction("TurnRight", EInputEvent::IE_Released, this, &ATank::TurnRightReleased);
	PlayerInputComponent->BindAction("TurnLeft", EInputEvent::IE_Pressed, this, &ATank::TurnLeftPressed);
	PlayerInputComponent->BindAction("TurnLeft", EInputEvent::IE_Released, this, &ATank::TurnLeftReleased);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Pressed, this, &ATank::Fire1Pressed);
	PlayerInputComponent->BindAction("Fire1", EInputEvent::IE_Released, this, &ATank::Fire1Released);
	PlayerInputComponent->BindAction("Fire2", EInputEvent::IE_Pressed, this, &ATank::Fire2Pressed);
	PlayerInputComponent->BindAction("Fire2", EInputEvent::IE_Released, this, &ATank::Fire2Released);
}

void ATank::MoveX(float AxisValue) {
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue) {
	TankInput.MoveY(AxisValue);
}

//Move straight section
void ATank::MoveForwardPressed()
{
	TankInput.MoveForward(true);
}

void ATank::MoveForwardReleased()
{
	TankInput.MoveForward(false);
}

void ATank::MoveBackwardPressed()
{
	TankInput.MoveBackward(true);
}

void ATank::MoveBackwardReleased()
{
	TankInput.MoveBackward(false);
}

//Spin around section
void ATank::TurnRightPressed()
{
	TankInput.TurnRight(true);
}

void ATank::TurnRightReleased()
{
	TankInput.TurnRight(false);
}

void ATank::TurnLeftPressed()
{
	TankInput.TurnLeft(true);
}

void ATank::TurnLeftReleased()
{
	TankInput.TurnLeft(false);
}

//Attack button section
void ATank::Fire1Pressed()
{
	TankInput.Fire1(true);
}

void ATank::Fire1Released()
{
	TankInput.Fire1(false);
}

void ATank::Fire2Pressed()
{
	TankInput.Fire2(true);
}

void ATank::Fire2Released()
{
	TankInput.Fire2(false);
}