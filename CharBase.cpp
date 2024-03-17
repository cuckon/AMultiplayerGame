// Fill out your copyright notice in the Description page of Project Settings.


#include "CharBase.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MpPlayerControllerBase.h"
#include "MpGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACharBase::ACharBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ReachingTarget = CreateDefaultSubobject<USceneComponent>(TEXT("ReachingTarget"));
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
	
	return Cast<ACharBase>(Result.GetActor());
}

void ACharBase::Release_Implementation(ACharBase* Player)
{
	CaughtPlayer = nullptr;
	ReachingTarget->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Player->CaughtByPlayers.Remove(this);
}

void ACharBase::Catch_Implementation(ACharBase* Player)
{
	CaughtPlayer = Player;
	ReachingTarget->AttachToComponent(Player->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Player->CaughtByPlayers.Add(this);
}

void ACharBase::HandleMoveInput_Implementation(const FVector3d& WorldDirection)
{
	this->AddMovementInput(WorldDirection);
}

void ACharBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharBase, CaughtPlayer);
}

// Called when the game starts or when spawned
void ACharBase::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		AMpGameModeBase* GameMode = Cast<AMpGameModeBase>(GetWorld()->GetAuthGameMode());
		GameMode->Players.Add(this);
	}
}

void ACharBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(HasAuthority())
	{
		AMpGameModeBase* GameMode = Cast<AMpGameModeBase>(GetWorld()->GetAuthGameMode());
		GameMode->Players.Remove(this);
	}
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
	ACharBase* FacingPlayer = GetFacingPlayer();
	if(FacingPlayer)
	{
	 	Catch(FacingPlayer);
	}
	CatchDelegate.Broadcast(FacingPlayer);
	bCatching = true;
}

void ACharBase::CatchCompletedCallback()
{
	if (CaughtPlayer)
	{
		Release(CaughtPlayer);
	}
	ReleaseDelegate.Broadcast(CaughtPlayer);
	bCatching = false;
}

