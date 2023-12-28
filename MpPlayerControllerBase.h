// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "MpPlayerControllerBase.generated.h"

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMpPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* IA_Move;
	
protected:
	virtual void BeginPlay() override;

	void SetupInput();

private:
	void InputHandler_Look(const FInputActionInstance& InputValue);
	void InputHandler_Move(const FInputActionInstance& InputValue);
};
