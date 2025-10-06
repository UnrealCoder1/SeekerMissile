// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "FPS_Base_Cpp/Components/IdentifierHolder.h"
#include "ValueSeeker.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_BASE_CPP_API UValueSeeker : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UValueSeeker();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	float MinimumAcceptanceValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	float MaximumSeekDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	float RefereshRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	float GimletLimit_Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	float GimletLimit_Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	UArrowComponent* GeneralSeekDirection_Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value Seeker")
	EValueTypes ValueType;

	EValueTypes ValueTypeToSeek;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckerFunction();

	FTimerHandle CheckerTimerHandle;
};
