// Fill out your copyright notice in the Description page of Project Settings.


#include "MpPlayerControllerBase.h"
#include "EnhancedInputSubsystems.h"
// #include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"


void AMpPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	SetupInput();
}

void AMpPlayerControllerBase::SetupInput()
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	if (InputSystem && InputMapping)
	{
		InputSystem->ClearAllMappings();
		InputSystem->AddMappingContext(InputMapping, 0);
	}
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if(EnhancedInputComponent && IA_Look && IA_Move)
	{
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMpPlayerControllerBase::InputHandler_Look);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMpPlayerControllerBase::InputHandler_Move);
	}
}

void AMpPlayerControllerBase::InputHandler_Look(const FInputActionInstance& InputValue)
{
	const FVector2d LookValue = InputValue.GetValue().Get<FVector2d>();
	GetPawn()->AddControllerYawInput(LookValue.X);
	GetPawn()->AddControllerPitchInput(LookValue.Y);
}

void AMpPlayerControllerBase::InputHandler_Move(const FInputActionInstance& InputValue)
{
	const FVector2d MoveValue = InputValue.GetValue().Get<FVector2d>();
	const FRotator Rotator = GetControlRotation();
	const FVector3d Movement = UKismetMathLibrary::GetRightVector(FRotator(0., Rotator.Yaw, Rotator.Roll)) * MoveValue.X +
		UKismetMathLibrary::GetForwardVector(FRotator(0., Rotator.Yaw,0.)) * MoveValue.Y;
	this->GetPawn()->AddMovementInput(Movement);
}
