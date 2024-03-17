// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "MpGameModeBase.generated.h"

class ACharBase;

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMpGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	friend class ACharBase;
	
public:
	AMpGameModeBase();
	
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void Solve();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
	bool bSolvePDB = true;

private:
	TArray<ACharBase*> Players;  // Players to move
};
