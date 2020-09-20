// Fill out your copyright notice in the Description page of Project Settings.

#include "TimerManager.h"
#include "PatrolComponent.h"
#include "PathsToChoose.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
// Sets default values for this component's properties
UPatrolComponent::UPatrolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPatrolComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPatrolComponent::FindNearestPath()
{
	FVector OwnerLocation;
	if(!GetOwner())
		return;
	OwnerLocation = GetOwner()->GetActorLocation();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APathsToChoose::StaticClass(), FoundActors);
	if(FoundActors.Num() <= 0)
		return;
	float dist = 999999999.f;
	int index = 0;
	for (int i = 0; i != FoundActors.Num(); i++)
	{
		float CurrentDistance = FVector::Dist(FoundActors[i]->GetActorLocation(), OwnerLocation);
		if (dist > CurrentDistance)
		{
			dist = CurrentDistance;
			index = i;
		}
	}
	TArray<AActor*> waypointsToBeAdded = Cast<APathsToChoose>(FoundActors[index])->GetWaypoints();
	waypoints.Empty();
	
	for (int i = 0; i != waypointsToBeAdded.Num(); i++)
	{
		waypoints.Add(waypointsToBeAdded[i]);
	}
}

