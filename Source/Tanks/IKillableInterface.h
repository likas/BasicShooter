// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tanks.h"
#include "CoreMinimal.h"
#include <Runtime\CoreUObject\Public\UObject\ObjectMacros.h>
#include "IKillableInterface.generated.h"


UINTERFACE(MinimalAPI) 
class UKillableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TANKS_API IKillableInterface
{
	GENERATED_BODY()

public:
	virtual void GetShot() = 0;
};
