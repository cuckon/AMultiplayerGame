// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"

#include "MpGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMpGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void Solve();
	
	void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	TArray<ACharacter*> Players;  // Players to move
};
