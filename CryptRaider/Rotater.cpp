// Fill out your copyright notice in the Description page of Project Settings.


#include "Rotater.h"

// Sets default values for this component's properties
URotater::URotater()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URotater::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetOwner()->GetActorRotation();
}


// Called every frame
void URotater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator TargetRotation = OriginalRotation;
	if (ShouldRotate)
	{
		TargetRotation = OriginalRotation + RotateOffset;
	}
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	float Speed = RotateOffset.Euler().Length() / RotateTime;

	FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, Speed);

	GetOwner()->SetActorRotation(NewRotation);
	if (RepeatableRotations && TargetRotation.Equals(GetOwner()->GetActorRotation()))
	{
		OriginalRotation = GetOwner()->GetActorRotation();
		ShouldRotate = false;
	}
}

void URotater::SetShouldRotate(bool Value)
{
	ShouldRotate = Value;
}