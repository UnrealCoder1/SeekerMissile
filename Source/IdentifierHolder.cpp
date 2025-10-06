// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Base_Cpp/Components/IdentifierHolder.h"
#include "GameFramework/Actor.h"
#include "UObject/NameTypes.h"

// Sets default values for this component's properties
UIdentifierHolder::UIdentifierHolder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	

	// ...
}


// Called when the game starts
void UIdentifierHolder::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (Owner == nullptr) { return; }

	Owner->Tags.Add(FName(
		StaticEnum<EValueTypes>()->GetNameStringByValue(static_cast<int64>(ValueType))
	)
	);
	// ...
	
}

