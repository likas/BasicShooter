// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "Wall.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API AWall : public AStaticMeshActor
{
	GENERATED_BODY()
public:
		AWall();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* MeshComponent;
};
