// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "Components/TimelineComponent.h"
#include "MissileOrRocketComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_BASE_CPP_API UMissileOrRocketComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UMissileOrRocketComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void ProjectileExplode();

    UFUNCTION()
    void EventOnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

    UFUNCTION()
    void OnProjectileRecieveAnyDamage(
        AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* InstigatedBy,
        AActor* DamageCauser
    );

    FTimerHandle EnableCollisionTimerHandle;

    /** Movement component (created/found at runtime, not in constructor) */
    UPROPERTY(Transient)
    UProjectileMovementComponent* Rocket_MissileProjectileMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float BaseDamage = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    bool bShouldExplodeInProximity = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float Proximity_Range = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    bool bShouldUseLaserTragetPositionIfValid = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float BodyInterpSpeed = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    FVector TargetLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float DamageRadius = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    bool IsRocket = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float Speed = 10000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float ProjectileGravityScale = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float TimeUntilCollisionActivation = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float TimeUntilDestroyingTheSmokeTrail = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generals")
    float TimeUntilOwnerDestruction = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* ExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TWeakObjectPtr<USceneComponent> SystemToDetach;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    USoundBase* ExplosionSound;

    UPROPERTY()
    TWeakObjectPtr<UNiagaraComponent> SmokeTrail;

    UPROPERTY()
    UTimelineComponent* MyTimeline;

    // Curve to drive the timeline (assign in editor or load dynamically)
    UPROPERTY(EditAnywhere, Category = "Timeline")
    UCurveFloat* FloatCurve;

    // Callback when timeline updates
    UFUNCTION()
    void TimelineProgress(float Value);

    void ExplodeInProximity();


    bool bHasExploded = false;

    AActor* ActorThatSpawnedTheOwner;

private:

    FTimerHandle ExplosionTimerHandle;

    FTimerHandle DestroySmokeTrail_TimerHandle;

    FTimerHandle DestroyOwner_TimerHandle;

    FOnTimelineFloat TimelineProgressFunction;

    FTimerHandle ExplodeInProximityTimerHandle;

    FOnTimelineEvent TimelineFinishedFunction;

};