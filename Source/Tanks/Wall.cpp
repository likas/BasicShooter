// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"


AWall::AWall()
{
	
	MeshComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Wall"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("Wall"));
}


