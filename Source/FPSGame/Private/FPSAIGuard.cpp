// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "FPSGameMode.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnHearingNoise);

}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();

	SetGuardState(EAIState::IDLE);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// When the another Pawn is seen
void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		return;
	}
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12.0f, FColor::Red, false, 10.0f);

	// If we have been spotted, fail the mission
	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::ALERTED);

}

void AFPSAIGuard::OnHearingNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::ALERTED)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12.0f, FColor::Blue, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewRotation.Pitch = 0.0f;
	NewRotation.Roll = 0.0f;

	SetActorRotation(NewRotation);

	GetWorldTimerManager().ClearTimer(TimeHandle_ResetRotation);
	
	GetWorldTimerManager().SetTimer(TimeHandle_ResetRotation, this, &AFPSAIGuard::ResetRotation, 3.0f);

	SetGuardState(EAIState::SUSPICIOUS);
	
}

void AFPSAIGuard::ResetRotation()
{
	if (GuardState == EAIState::ALERTED)
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::IDLE);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;

	OnStateChange(NewState);
}

