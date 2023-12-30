// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppJoinDeclarationAndAssignment
#include "MpGameModeBase.h"
#include "CharBase.h"
#include "Kismet/GameplayStatics.h"

using TLinks = TArray<TPair<int, int>>; 

//TODO: Make it 2d to potentially speed it up
void SolvePDB(
	const TArray<FVector3d> &Points,
	TLinks Links,
	TArray<FVector3d>& OutVelocities,
	const float RestLength=1,
	const int N=10
	)
{
	// Initialize
	float Length, Error;
	int P_Another;
	FVector3d Point_Another, Delta, Movement;
	OutVelocities.Reset();

	TArray<FVector3d> Points_New;
	for (const auto& Point: Points)
	{
		Points_New.Emplace(Point);
		OutVelocities.Emplace(FVector3d::ZeroVector);
	}

	// Solve
	for (int iSubstep = 0; iSubstep < N; ++iSubstep)
	{
		for (int i = 0; i < Points_New.Num(); ++i)
		{
			FVector3d& Point = Points_New[i];

			for (const auto& Link: Links)
			{
				// TODO: cache this
				if (Link.Key == i)
				{
					P_Another = Link.Value;
				} else if (Link.Value == i)
				{
					P_Another = Link.Key;
				} else
				{
					continue;
				}

				Point_Another = Points[P_Another];
				Delta = Point - Point_Another;
				Length = Delta.Length() / 100;  // cm -> m
				Error = (RestLength - Length) * .5;
				Movement = Delta.GetSafeNormal() * Error;
			}
			Point += Movement;
		}
	}

	// Calculate velocity
	for (int i = 0; i < Points_New.Num();++i)
		OutVelocities[i] = Points_New[i] - Points[i];
}


void BuildLinksData(const TArray<ACharBase*> &Players, TLinks& Result)
{
	Result.Reset();
	TPair<int, int> Link;
	for (int i = 0; i<Players.Num(); ++i)
	{
		if (Players[i]->CaughtPlayer)
		{
			Link = TPair<int, int>(i, Players.Find(Players[i]->CaughtPlayer));
			Result.Add(Link);
		}
	}
}

void DrawLinks(TArray<ACharBase*> Players, TLinks Links, const UWorld* World)
{
	for (const auto& Link: Links)
	{
		DrawDebugLine(
			World,
			Players[Link.Key]->GetActorLocation(),
			Players[Link.Value]->GetActorLocation(),
			FColor::Red, false, 0.05f);
	}
}

void AMpGameModeBase::Solve_Implementation()
{
	// Prepare data
	TLinks Links;
	BuildLinksData(Players, Links);
	TArray<FVector3d> Points, Velocities;
	for (const auto Player : Players)
		Points.Add(Player->GetActorLocation());

	// Solve
	SolvePDB(Points, Links, Velocities);

	// Apply velocities
	for (int i = 0; i<Players.Num(); ++i)
		Players[i]->HandleMoveInput(Velocities[i]);

	// Draw links
	DrawLinks(Players, Links, GetWorld());
}

AMpGameModeBase::AMpGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void UpdatePlayers(TArray<ACharBase*>& Players, UWorld* World)
{
	TArray<AActor*> FoundPlayers;
	Players.Reset();
	
	UGameplayStatics::GetAllActorsOfClass(
		World, ACharBase::StaticClass(), FoundPlayers);
	for (const auto i: FoundPlayers)
		if (ACharBase* CastedPlayer = Cast<ACharBase>(i))
			Players.Add(CastedPlayer);
}

void AMpGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Solve();
}
