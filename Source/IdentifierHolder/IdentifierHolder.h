// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IdentifierHolder.generated.h"


UENUM(BlueprintType)
enum class EValueTypes : uint8
{
	Heat UMETA(DisplayName = "Heat"),
	Radiation UMETA(DisplayName = "Radiation"),
	Miscellaneous UMETA(DisplayName = "Miscellaneous")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_BASE_CPP_API UIdentifierHolder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIdentifierHolder();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* IdentifierComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	EValueTypes ValueType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float Intensity = 10.0f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

		
};
