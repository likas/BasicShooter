// Fill out your copyright notice in the Description page of Project Settings.

#include "TankStatics.h"
#include "Tanks.h"


/* Find the smallest angle between two headings (in degrees) */
float UTankStatics::FindDeltaAngleDegrees(float A1, float A2) {
	// Find the difference
	float Delta = A2 - A1;
	//if change is large than 180
	if (Delta > 180.f)
	{
		//flip to negative equivalent
		Delta = Delta - 360.f;
	} 
	else if (Delta < -180.f)
	{
		Delta = Delta + 360.f;
	}

	return Delta;
}

bool UTankStatics::FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float& Angle) {
	// Normalization is not necessary
	FVector2D Normal = (Target - Start).GetSafeNormal();
	if (!Normal.IsNearlyZero()) {
		Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
		return true;
	}
	return false;
}