// Fill out your copyright notice in the Description page of Project Settings.


#include "CharBase.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MpPlayerControllerBase.h"
#include "MpGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

const double AlignToCaughtRate = 10.0;

// Sets default values
ACharBase::ACharBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ReachingTarget = CreateDefaultSubobject<USceneComponent>(TEXT("ReachingTarget"));
	ReachingTarget->SetIsReplicated(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 8.5f));
	SpringArm->TargetArmLength = 400.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
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
	bCatching = false;
}

void ACharBase::Catch_Implementation(ACharBase* Player)
{
	CaughtPlayer = Player;
	ReachingTarget->AttachToComponent(Player->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Player->CaughtByPlayers.Add(this);

	bCatching = true;
}

void ACharBase::HandleMoveInput_Implementation(const FVector3d& WorldDirection)
{
	this->AddMovementInput(WorldDirection);
}

void ACharBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharBase, CaughtPlayer);
	DOREPLIFETIME(ACharBase, bCatching);
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

	GetCharacterMovement()->bUseControllerDesiredRotation = (
		GetCharacterMovement()->GetLastUpdateVelocity().Length() > 1   // moving
		&& CaughtByPlayers.IsEmpty()	// not caught
		&& !IsValid(CaughtPlayer) // caught any player 
		);

	if (CaughtPlayer)
	{
		const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), CaughtPlayer->GetActorLocation()
		);

		const FRotator DesiredRotation = FMath::RInterpTo(GetActorRotation(), Rotation, DeltaTime, AlignToCaughtRate);
		
		GetCharacterMovement()->MoveUpdatedComponent(
			FVector::ZeroVector, DesiredRotation,
			/*bSweep*/ false );
	}
}

// Called to bind functionality to input
void ACharBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		Cast<APlayerController>(GetController())->GetLocalPlayer());
	if (InputSystem && InputMapping)
		InputSystem->AddMappingContext(InputMapping, 0);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACharBase: Input mapping not found."));
	}
	
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
	CatchDelegate.Broadcast(this, FacingPlayer);
}

void ACharBase::CatchCompletedCallback()
{
	if (CaughtPlayer)
	{
		Release(CaughtPlayer);
	}
	ReleaseDelegate.Broadcast(this, CaughtPlayer);
}

