// Fill out your copyright notice in the Description page of Project Settings.


#include "TanksGameInstance.h"

int32 UTanksGameInstance::GetTankScore() const
{
	return TankScore;
}

int32 UTanksGameInstance::GetBotScore() const
{
	return BotScore;
}

void UTanksGameInstance::SetTankScore()
{
	TankScore += 1;
}

void UTanksGameInstance::SetBotScore()
{
	BotScore += 1;
}