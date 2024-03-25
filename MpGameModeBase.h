// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "MpGameModeBase.generated.h"

class ACharBase;
class AEagle;
class Hen;

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

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	TArray<ACharBase*> Players;  // Players to move

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EagleCaughtChicken(ACharBase* Eagle, ACharBase* Hen);
};
