// Fill out your copyright notice in the Description page of Project Settings.


#include "TanksGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Tank.h"
#include "TanksGameInstance.h"
#include "Bot.h"

ATanksGameMode::ATanksGameMode() 
{
	TankScore = 0;
	BotScore = 0;
}

void ATanksGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATank* MyCharacter = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	/*TArray<AActor> *Bots;
	UGameplayStatics::GetAllActorsOfClass(this, ABot::StaticClass(), Bots);*/

	UTanksGameInstance* TGI;


	ABot* Bot = Cast<ABot>( UGameplayStatics::GetActorOfClass(this, ABot::StaticClass()) );
	if (MyCharacter->IsDead())
	{
		TGI = Cast<UTanksGameInstance>(GetGameInstance());
		TGI->SetBotScore();
		UE_LOG(LogTemp, Log, TEXT("Tank death registered"));
		RestartGame();
		//RestartPlayerAtTransform(MyCharacter->GetController(), MyCharacter->GetStartPoint());

		//RestartPlayerAtPlayerStart(Bot->GetController());
	}

	if (Bot->IsDead())
	{
		TGI = Cast<UTanksGameInstance>(GetGameInstance());
		TGI->SetTankScore();
		UE_LOG(LogTemp, Log, TEXT("Bot death registered"));
		RestartGame();

	}
}

void ATanksGameMode::BeginPlay() 
{
	Super::BeginPlay();

	if (HUDWigdetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWigdetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}

}

int32 ATanksGameMode::GetTankScore() const
{
	return TankScore;
}

int32 ATanksGameMode::GetBotScore() const
{
	return BotScore;
}

void ATanksGameMode::SetTankScore()
{
	TankScore += 1;
}

void ATanksGameMode::SetBotScore()
{
	BotScore += 1;
}