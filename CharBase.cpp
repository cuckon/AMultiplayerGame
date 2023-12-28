// Fill out your copyright notice in the Description page of Project Settings.


#include "CharBase.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MpPlayerControllerBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

// Sets default values
ACharBase::ACharBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ACharBase* ACharBase::GetFacingPlayer() const
{
	FHitResult Result;
	FVector3d Start = GetActorLocation();
	FVector3d End = Start + GetActorForwardVector() * CatchDistance * 100;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(
		Result, GetActorLocation(), End,
		ECollisionChannel::ECC_GameTraceChannel1,
		CollisionQueryParams);

	if (!Result.bBlockingHit)
		return nullptr;

	DrawDebugLine(
			GetWorld(),
			Start,
			Result.Location,
			FColor(50, 190, 0),
			false, 1, 0,
			12.333
		);
	return Cast<ACharBase>(Result.GetActor());
}

void ACharBase::Release_Implementation(ACharBase* Player)
{
	CaughtPlayer = nullptr;
	Player->CaughtByPlayers.Remove(this);
}

void ACharBase::Catch_Implementation(ACharBase* Player)
{
	CaughtPlayer = Player;
	Player->CaughtByPlayers.Add(this);
}

void ACharBase::HandleMoveInput_Implementation(const FVector3d& WorldDirection)
{
	this->AddMovementInput(WorldDirection);
}

// Called when the game starts or when spawned
void ACharBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		Cast<APlayerController>(GetController())->GetLocalPlayer());
	if (InputSystem && InputMapping)
		InputSystem->AddMappingContext(InputMapping, 0);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if(EnhancedInputComponent && IA_Catch)
	{
		EnhancedInputComponent->BindAction(
			IA_Catch, ETriggerEvent::Started, this, &ACharBase::CatchStartedCallback);
		EnhancedInputComponent->BindAction(
			IA_Catch, ETriggerEvent::Completed, this, &ACharBase::CatchCompletedCallback);
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACharBase: Not bound."));
	}
}

void ACharBase::CatchStartedCallback()
{
	 if(ACharBase* FacingPlayer = GetFacingPlayer())
	 {
	 	Catch(FacingPlayer);
	 	UE_LOG(LogTemp, Log, TEXT("Caught!"));
	 }
}

void ACharBase::CatchCompletedCallback()
{
	if (CaughtPlayer)
	{
		Release(CaughtPlayer);
		UE_LOG(LogTemp, Log, TEXT("Release!"));
	}
}

