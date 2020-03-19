// Fill out your copyright notice in the Description page of Project Settings.


#include "TanksGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Tank.h"
#include "TanksGameInstance.h"
#include "Bot.h"


void ATanksGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATank* MyCharacter = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	ABot* Bot = Cast<ABot>(UGameplayStatics::GetActorOfClass(this, ABot::StaticClass()));
	/*
	// TODO multiple bots
	TArray<AActor*> Bots;
	UGameplayStatics::GetAllActorsOfClass(this, ABot::StaticClass(), Bots);
	*/

	UTanksGameInstance* TGI;
	
	/* If someone get shot, increase opposite side's points, restart the game */

	if (MyCharacter->IsDead())
	{
		TGI = Cast<UTanksGameInstance>(GetGameInstance());
		TGI->SetBotScore();
		UE_LOG(LogTemp, Log, TEXT("Tank death registered"));
		RestartGame();
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