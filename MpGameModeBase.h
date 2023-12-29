// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"

#include "MpGameModeBase.generated.h"

class ACharBase;

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMpGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMpGameModeBase();
	
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void Solve();
	
	void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	TArray<ACharBase*> Players;  // Players to move
};
