// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Base_Cpp/Components/ValueSeeker.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Algo/MaxElement.h"
#include "FPS_Base_Cpp/Explosives/MissileOrRocketComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UValueSeeker::UValueSeeker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UValueSeeker::BeginPlay()
{
	Super::BeginPlay();
	ValueTypeToSeek = ValueType;

	GetWorld()->GetTimerManager().SetTimer(
		CheckerTimerHandle,
		this,
		&UValueSeeker::CheckerFunction,
		RefereshRate,
		true
	);
	// ...
	
}


// Called every frame
void UValueSeeker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UValueSeeker::CheckerFunction()
{

	FName TagToSeek = FName(
		StaticEnum<EValueTypes>()->GetNameStringByValue(static_cast<int64>(ValueTypeToSeek))
	);

	TArray<AActor*> PossibleTargets;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TagToSeek, PossibleTargets);

	if (PossibleTargets.Num() == 0) { return; }

	TArray<AActor*> Targets;

	TArray<float> Intensities;

	for (TWeakObjectPtr<AActor> SelectedElem : PossibleTargets)
	{
		if (SelectedElem.IsValid())
		{
			if(UIdentifierHolder* IdentifierHolder = SelectedElem->FindComponentByClass<UIdentifierHolder>())
			{
				float Distance = UKismetMathLibrary::Vector_Distance(
					SelectedElem->GetActorLocation(),
					GeneralSeekDirection_Arrow->GetComponentLocation()
				);

				FRotator DirectionToTarget = UKismetMathLibrary::FindLookAtRotation(
					GeneralSeekDirection_Arrow->GetComponentLocation(),
					SelectedElem->GetActorLocation()
				);

				FRotator GeneralSeekDirection = GeneralSeekDirection_Arrow->GetComponentRotation();

				if (UKismetMathLibrary::Abs(DirectionToTarget.Yaw - GeneralSeekDirection.Yaw) <= GimletLimit_Z &&
					UKismetMathLibrary::Abs(DirectionToTarget.Pitch - GeneralSeekDirection.Pitch) <= GimletLimit_Y)
				{
					FVector TraceStart = GeneralSeekDirection_Arrow->GetComponentLocation();
					FVector TraceEnd = TraceStart + (DirectionToTarget.Vector() * (Distance + 1000.0f));
					FHitResult HitResult;

					UKismetSystemLibrary::LineTraceSingle(
						GetWorld(),
						TraceStart,
						TraceEnd,
						UEngineTypes::ConvertToTraceType(ECC_Visibility),
						false,
						TArray<AActor*>{GetOwner()},
						EDrawDebugTrace::None,
						HitResult,
						true
					);

					if (HitResult.bBlockingHit && HitResult.GetActor() != nullptr)
					{
						if (IdentifierHolder->Intensity >= MinimumAcceptanceValue && Distance <= MaximumSeekDistance && HitResult.GetActor() == SelectedElem)
						{
							if (!Targets.Contains(SelectedElem.Get()))
							{
								Intensities.Add(IdentifierHolder->Intensity * UKismetMathLibrary::MapRangeClamped(HitResult.Distance, 0.0f, MaximumSeekDistance, 1.0f, 0.02f));
								Targets.Add(SelectedElem.Get());
							}
						}
						else
						{
							if(Targets.Contains(SelectedElem.Get()))
							{
								Targets.Remove(SelectedElem.Get());
							}
						}
					}
				}
			}
		}
	}

	if (Targets.Num() == 0) { return; }

	AActor* Target = nullptr;

	int idx = -1;

	for (float intensity : Intensities)
	{
		idx++;
		if (intensity == *Algo::MaxElement(Intensities))
		{
			Target = Targets[idx];
		}
	}

	if (Target != nullptr)
	{
		if(GetOwner()->FindComponentByClass<UMissileOrRocketComponent>())
		{
			if (UMissileOrRocketComponent* MissileOrRocketComponent = GetOwner()->FindComponentByClass<UMissileOrRocketComponent>())
			{
				if (MissileOrRocketComponent->IsRocket == false)
				{
					MissileOrRocketComponent->TargetLocation = Target->GetActorLocation();
					MissileOrRocketComponent->bShouldUseLaserTragetPositionIfValid = false;
				}
			}
		}
	}
}

