// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "Tanks.h"
//#include "Bot.h"
#include "Missile.h"
#include "Wall.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
//#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h"


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

//Exiting game
void FTankInput::Esc(bool bPressed)
{
	bEsc = bPressed;
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

	/* Arrow pointing in the direction where the tank facing */
	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->SetupAttachment(RootComponent);

	/* This is the picture of the tank (player) */
	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->SetupAttachment(TankDirection);

	/* This is the body, later we determine our size based on it */
	TankBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TankBody"));
	TankBody->SetupAttachment(TankDirection);
	TankBody->SetBoxExtent(FVector(40.0f, 40.0f, 100.0f));
	TankBody->SetCollisionProfileName(TEXT("Tank:Move")); 

	/* SpringArm is auxiliary component that "pins" a camera to a player. Kinda like a selfie stick */
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->CameraLagSpeed = 2.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-90.f, 0.f, 0.f)); // Fixed on creation

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.f;
	CameraComponent->AspectRatio = 1.f;
	CameraComponent->bConstrainAspectRatio = true;
	CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(0.f, 0.f, 0.f)); // Fixed on creation

	MoveSpeed = 100.0f;
	YawSpeed = 150.0f;
	Fire1Cooldown = 0.5f;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	bIsKilled = false;
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FTankInput& CurrentInput = GetCurrentInput();

	// Respond to controls if we're not dead.
	if (bIsKilled == false)
	{
		if (CurrentInput.bEsc) 
		{

			APlayerController* PC = Cast<APlayerController>(GetController());
			UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
			
		}
		if (CurrentInput.bMoveForward || CurrentInput.bMoveBackward)
		{
			/* Get direction vector */
			FVector Direction = TankDirection->GetForwardVector() * (CurrentInput.bMoveForward ? 1 : -1);

			/* This is where we are */
			FVector Pos = GetActorLocation();
			/* This is where we want to be */
			FVector DesiredLoc = Pos + Direction * MoveSpeed * DeltaTime;

			/* Check if something is in the way */
			if (UWorld* World = GetWorld())
			{
				TArray<FHitResult> HitResults;
				FVector BoxSize = TankBody->GetScaledBoxExtent();
				FCollisionShape CollisionShape;
				CollisionShape.SetBox(BoxSize);

				/* 
				Sweep is somewhat like a ray trace. We Sweep given shape through the world and see if it hit something
				Results are stored in HitResults array; we sweep from Pos to DesiredLoc, giving also the rotation in the quarternion form,
				custom collision profile ("Tank:Move"), and the shape of the tank
				*/

				if (World->SweepMultiByProfile(HitResults, Pos, DesiredLoc, TankBody->GetComponentRotation().Quaternion(), "Tank:Move", CollisionShape))
				{
					/* 
					Hit something that blocks us
					So do nothing, just don't move
					(If hit by missile, it itself handles the collision through interface)
					*/
				}
				else
				{
					/* Add movement vector to our current position */
					SetActorLocation(DesiredLoc);
				}
			}

		}

		/* Handle rotation input */
		if (CurrentInput.bTurnLeft || CurrentInput.bTurnRight)
		{
			//TODO Make it frame-independent
			float FrameIndependentYawSpeed = YawSpeed * DeltaTime;
			float DesiredDeltaYaw = CurrentInput.bTurnRight != CurrentInput.bMoveBackward ? FrameIndependentYawSpeed : -1.f * FrameIndependentYawSpeed;
			FRotator NewRotation = FRotator(0.f, DesiredDeltaYaw, 0.f);
			FQuat QuatRotation = FQuat(NewRotation);
			TankDirection->AddLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		}

		//Shoot
		if (UWorld* World = GetWorld()) {
			float CurrentTime = World->GetTimeSeconds();
			if (CurrentInput.bFire1 && Fire1ReadyTime <= CurrentTime)
			{
				FVector Loc = GetActorLocation() + FVector(TankDirection->GetForwardVector().X * 70.f, TankDirection->GetForwardVector().Y * 70.f, 0.f);
				FRotator Rot = TankDirection->GetComponentRotation();
				if (AActor* NewProjectile = World->SpawnActor(Projectile))
				{
					UE_LOG(LogTemp, Log, TEXT("Projectile spawned by Player"));
					NewProjectile->SetActorLocation(Loc);
					NewProjectile->SetActorRotation(Rot);
				}

				// Sets the cooldown timer.
				Fire1ReadyTime = CurrentTime + Fire1Cooldown;
			}
		}
	}
}

void ATank::GetShot()
{
	bIsKilled = true;
	UE_LOG(LogTemp, Log, TEXT("Tank was killed"));
}

bool ATank::IsDead() 
{
	return bIsKilled;
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	PlayerInputComponent->BindAction("Quit", EInputEvent::IE_Pressed, this, &ATank::EscPressed);
	PlayerInputComponent->BindAction("Quit", EInputEvent::IE_Released, this, &ATank::EscReleased);
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

//Buttons to quit game
void ATank::EscPressed()
{
	TankInput.Esc(true);
}

void ATank::EscReleased()
{
	TankInput.Esc(false);
}