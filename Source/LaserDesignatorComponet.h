// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ArrowComponent.h"
#include "LaserDesignatorComponet.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_BASE_CPP_API ULaserDesignatorComponet : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UArrowComponent* TraceDirectArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
	float RayLength;

	FVector LaserTargetLocation;

	FTimerHandle FireLaserRayTimerHandle;

	// Sets default values for this component's properties
	ULaserDesignatorComponet();

	void FireTrace();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
