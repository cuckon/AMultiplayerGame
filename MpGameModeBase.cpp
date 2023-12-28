// Fill out your copyright notice in the Description page of Project Settings.


#include "MpGameModeBase.h"

using TLink = TMap<int, int>;
using TLinks = TArray<TLink>; 

void SolvePDB(
	TArray<FVector3d> &Points,
	TLinks Links,
	const float RestLength=1,
	const int N=10
	)
{
	float Length, Error, Weight = 0.f;
	int P_Another;
	FVector3d Point_Another, Delta, Movement;

	for (int iSubstep = 0; iSubstep < N; ++iSubstep)
	{
		for (int i = 0; i < Points.Num(); ++i)
		{
			FVector3d& Point = Points[i];
			for (const TLink& Link: Links)
			{
				// Todo: cache this
				if (Link[0] == i)
				{
					P_Another = Link[1];
				} else if (Link[1] == i)
				{
					P_Another = Link[0];
				} else
				{
					continue;
				}

				Point_Another = Points[P_Another];
				Delta = Point - Point_Another;
				Length = Delta.Length();
				Error = (RestLength - Length) * .5;
				Movement = Delta.GetSafeNormal() * Error;
				Weight += 1.0;
			}
			// Point += Movement / Weight;
			Point += Movement;
		}
	}
}

void AMpGameModeBase::Solve_Implementation()
{
	
}

void AMpGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	// Players = 
}

void AMpGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
