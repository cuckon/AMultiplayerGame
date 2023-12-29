// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "CharBase.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class MULTIPLAYER_API ACharBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharBase();

	UFUNCTION(Client, Unreliable)
	void HandleMoveInput(const FVector3d& WorldDirection);

	UFUNCTION(Server, Reliable)
	void Catch(ACharBase* Player);

	UFUNCTION(Server, Reliable)
	void Release(ACharBase* Player);

	UFUNCTION(BlueprintCallable)
	ACharBase* GetFacingPlayer() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Catch Distance (meters)")
	double CatchDistance = 1.0;
	
	UPROPERTY(BlueprintReadOnly)
	ACharBase* CaughtPlayer;

	UPROPERTY(BlueprintReadOnly)
	TArray<ACharBase*> CaughtByPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* IA_Catch;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void CatchStartedCallback();
	void CatchCompletedCallback();
};
