// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "Rotater.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetPhysicsHandle() && GetPhysicsHandle()->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		GetPhysicsHandle()->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Release()
{
	if (GetPhysicsHandle() && GetPhysicsHandle()->GetGrabbedComponent())
	{
		AActor* GrabbedActor = GetPhysicsHandle()->GetGrabbedComponent()->GetOwner();
		GrabbedActor->Tags.Remove("Grabbed");
		GetPhysicsHandle()->ReleaseComponent();
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult;
	bool DidReach = GetGrabbableInReach(HitResult);
	UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	AActor* HitActor = HitResult.GetActor();

	if (GetPhysicsHandle() && DidReach && !HitActor->ActorHasTag("Click"))
	{
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HitActor->Tags.Add("Grabbed");
		GetPhysicsHandle()->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);
	}
	else if (DidReach && HitActor->ActorHasTag("Click"))
	{
		URotater* ActorRotater = HitActor->FindComponentByClass<URotater>();
		if (ActorRotater)
		{
			ActorRotater->SetShouldRotate(true);
		}
		if (HitActor->ActorHasTag("Lever") && !HitActor->ActorHasTag("Pulled"))
		{
			HitActor->Tags.Add("Pulled");
		}
	}
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	DrawDebugSphere(GetWorld(), End, 10, 10, FColor::Red, false, 5);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	return GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere
	);
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle was found, Grabber requires a UPhysicsHandleComponent."));
	}
	return PhysicsHandle;
}