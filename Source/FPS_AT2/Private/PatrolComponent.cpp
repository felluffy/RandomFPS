// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolComponent.h"

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

