// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Base_Cpp/Components/LaserDesignatorComponet.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
ULaserDesignatorComponet::ULaserDesignatorComponet()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULaserDesignatorComponet::FireTrace()
{
	FHitResult HitResult;
	FVector TraceStart = TraceDirectArrow->GetComponentLocation();
	FVector TraceEnd = TraceStart + TraceDirectArrow->GetComponentRotation().Vector() * RayLength;

	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		TraceStart,
		TraceEnd,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ GetOwner() },
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
	);

	if (HitResult.bBlockingHit)
	{
		LaserTargetLocation = HitResult.ImpactPoint;
	}
	else
	{
		LaserTargetLocation = HitResult.TraceEnd;
	}
}


// Called when the game starts
void ULaserDesignatorComponet::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(
		FireLaserRayTimerHandle,
		this,
		&ULaserDesignatorComponet::FireTrace,
		0.1f,
		true
	);
	// ...
	
}


// Called every frame
void ULaserDesignatorComponet::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

