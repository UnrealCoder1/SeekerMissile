// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Base_Cpp/Explosives/MissileOrRocketComponent.h"
#include "FPS_Base_Cpp/ProjectFunctionLibrary/ShooterBaseFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#include "FPS_Base_Cpp/Components/RocketLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values for this component's properties
UMissileOrRocketComponent::UMissileOrRocketComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Do NOT create UProjectileMovementComponent here — not allowed in ActorComponent constructor
    Rocket_MissileProjectileMovement = nullptr;

    MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MyTimeline"));
    MyTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
}

// Called when the game starts
void UMissileOrRocketComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    Owner->SetActorEnableCollision(false);

    // Try to find existing movement component
    Rocket_MissileProjectileMovement = Owner->FindComponentByClass<UProjectileMovementComponent>();

    // If none exists, create one and attach it
    if (!Rocket_MissileProjectileMovement)
    {
        Rocket_MissileProjectileMovement = NewObject<UProjectileMovementComponent>(
            Owner,
            UProjectileMovementComponent::StaticClass(),
            TEXT("RocketProjectileMovement")
        );

        if (Rocket_MissileProjectileMovement)
        {
            Rocket_MissileProjectileMovement->RegisterComponent();
            Rocket_MissileProjectileMovement->UpdatedComponent = Owner->GetRootComponent();
            Rocket_MissileProjectileMovement->ProjectileGravityScale = ProjectileGravityScale;
            Rocket_MissileProjectileMovement->bShouldBounce = true;
            Rocket_MissileProjectileMovement->bRotationFollowsVelocity = false;
        }
    }

    FTimerDelegate EnableCollision_TimerDelegate;
    EnableCollision_TimerDelegate.BindLambda([this]()
        {
            GetOwner()->SetActorEnableCollision(true);
        }
    );

    GetWorld()->GetTimerManager().SetTimer(EnableCollisionTimerHandle, EnableCollision_TimerDelegate, TimeUntilCollisionActivation, false);

    GetWorld()->GetTimerManager().SetTimer(ExplodeInProximityTimerHandle, this, &UMissileOrRocketComponent::ExplodeInProximity, 0.06f, true);

    // Initialize velocity if it's a rocket
    if (IsRocket && Rocket_MissileProjectileMovement)
    {
        Rocket_MissileProjectileMovement->Velocity = Owner->GetActorForwardVector() * Speed;
    }

    // Bind bounce event
    if (Rocket_MissileProjectileMovement)
    {
        Rocket_MissileProjectileMovement->OnProjectileBounce.AddDynamic(this, &UMissileOrRocketComponent::EventOnProjectileBounce);
    }

    // Bind damage event
    Owner->OnTakeAnyDamage.AddDynamic(this, &UMissileOrRocketComponent::OnProjectileRecieveAnyDamage);


    if (TimeUntilOwnerDestruction != 0.0f)
    {
        FTimerDelegate DestroyActor_TimerDel;
        DestroyActor_TimerDel.BindLambda([this]()
            {
                ProjectileExplode();
            }
        );

        GetWorld()->GetTimerManager().SetTimer(
            DestroyOwner_TimerHandle,
            DestroyActor_TimerDel,
            TimeUntilOwnerDestruction,
            false
        );
    }
}

// Called every frame
void UMissileOrRocketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


    if (Rocket_MissileProjectileMovement != nullptr)
    {
        if (IsRocket == false)
        {
            if (TargetLocation == FVector::ZeroVector)
            {
                Rocket_MissileProjectileMovement->Velocity = Rocket_MissileProjectileMovement->UpdatedComponent->GetForwardVector() * Speed;
            }
            else
            {
                GetOwner()->SetActorRotation(
                    UKismetMathLibrary::RInterpTo_Constant(
                        GetOwner()->GetActorRotation(),
                        UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation),
                        DeltaTime,
                        BodyInterpSpeed
                    )
                );
                Rocket_MissileProjectileMovement->Velocity = GetOwner()->GetActorForwardVector() * Speed;
            }
        }
    }
    if (ActorThatSpawnedTheOwner)
    {
        if (URocketLauncher* RocketLauncher = ActorThatSpawnedTheOwner->FindComponentByClass<URocketLauncher>())
        {
            if (RocketLauncher->bHasLaser == true && IsRocket == false)
            {
                TargetLocation = RocketLauncher->LaserRecievedTargetLocation;
                UKismetSystemLibrary::PrintString(GetWorld(), "Recieving");
            }
        }
    }
}

void UMissileOrRocketComponent::ProjectileExplode()
{
    if (bHasExploded) { return; }

    bHasExploded = true;

    if (SystemToDetach != nullptr)
    {
        SmokeTrail = Cast<UNiagaraComponent>(SystemToDetach);

        SystemToDetach->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

        SystemToDetach->Rename(nullptr, GetWorld());

        if (FloatCurve)
        {
            TimelineProgressFunction.BindUFunction(this, FName("TimelineProgress"));

            MyTimeline->AddInterpFloat(FloatCurve, TimelineProgressFunction);

            MyTimeline->SetLooping(false);

            MyTimeline->PlayFromStart();
        }
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return; // or handle gracefully
    }

    UShooterBaseFunctions::GenerateGeneralExplosion(
        GetWorld(),
        BaseDamage,
        Owner,
        DamageRadius,
        ExplosionSound,
        ExplosionEffect,
        true,
        Owner->GetActorLocation()
    );

    if (SystemToDetach != nullptr)
    {
        FTimerDelegate DestroySystemTimerDel;
        DestroySystemTimerDel.BindLambda([this]()
            {
                if (SystemToDetach == nullptr) { return; }
                SystemToDetach->DestroyComponent();
            }
        );

        GetWorld()->GetTimerManager().SetTimer(
            DestroySmokeTrail_TimerHandle,
            DestroySystemTimerDel,
            TimeUntilDestroyingTheSmokeTrail,
            false
        );
    }
}

void UMissileOrRocketComponent::EventOnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
    if (bHasExploded) { return; }

    if (ImpactResult.GetActor()->GetClass() != GetOwner()->GetClass())
    {
        ProjectileExplode();
    }
    else
    {
        GetOwner()->Destroy();
    }
}

void UMissileOrRocketComponent::OnProjectileRecieveAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (bHasExploded) { return; }

    if (DamageCauser->GetClass() != GetOwner()->GetClass())
    {
        ProjectileExplode();
    }
    else
    {
        GetOwner()->Destroy();
    }
}

void UMissileOrRocketComponent::TimelineProgress(float Value)
{
    if (SmokeTrail != nullptr)
    {
        SmokeTrail->SetVariableInt("SpawnRate", FMath::Lerp(400, 0, Value));
    }
}

void UMissileOrRocketComponent::ExplodeInProximity()
{
    if (!bShouldExplodeInProximity) return;

    float DistanceToTarget = UKismetMathLibrary::Vector_Distance(GetOwner()->GetActorLocation(), TargetLocation);

    if (DistanceToTarget <= Proximity_Range)
    {
        ProjectileExplode();
    }
}
