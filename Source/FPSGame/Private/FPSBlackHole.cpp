// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBlackHole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComp;
    
    InnerSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphere"));
    InnerSphereComponent->SetSphereRadius(100);
    InnerSphereComponent->SetupAttachment(MeshComp);
    
    InnerSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::OverlapInnerSphere);
    
    OuterSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphere"));
    OuterSphereComponent->SetSphereRadius(3000);
    OuterSphereComponent->SetupAttachment(MeshComp);
    
}

void AFPSBlackHole::OverlapInnerSphere(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
                                       int32 OtherBodyIndex, bool bfromSweep, const FHitResult &SweepResult)
{
        if(OtherActor)
        {
            OtherActor->Destroy();
        }
}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    TArray<UPrimitiveComponent*> OverlappingComponents;
    OuterSphereComponent->GetOverlappingComponents(OverlappingComponents);
    
    for(int32 i = 0; i < OverlappingComponents.Num(); i++)
    {
        UPrimitiveComponent* PrimComp = OverlappingComponents[i];
        if(PrimComp && PrimComp->IsSimulatingPhysics())
        {
            const float SphereRadius = OuterSphereComponent->GetScaledSphereRadius();
            const float ForceStrength = -2000;
            
            PrimComp->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);
        }
    }

}

