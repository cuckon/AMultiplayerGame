// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "CharBase.generated.h"

class UInputMappingContext;
class UInputAction;
class ACharBase;

/** Representation of a catchable and caughtable player's character
 */
UCLASS()
class MULTIPLAYER_API ACharBase : public ACharacter
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharBaseDeletegate, ACharBase*, Target);

	// Sets default values for this character's properties
	ACharBase();

	UFUNCTION(Client, Unreliable)
	void HandleMoveInput(const FVector3d& WorldDirection);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Catch")
	void Catch(ACharBase* Player);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Catch")
	void Release(ACharBase* Player);

	UFUNCTION(BlueprintCallable)
	ACharBase* GetFacingPlayer() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Catch")
	bool bCatching = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Catch Distance (meters)", Category = "Catch")
	double CatchDistance = 1.0;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Catch")
	ACharBase* CaughtPlayer;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Catch")
	TArray<ACharBase*> CaughtByPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* IA_Catch;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Catch")
	FCharBaseDeletegate CatchDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Catch")
	FCharBaseDeletegate ReleaseDelegate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Catch")
	USceneComponent* ReachingTarget;
	
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
