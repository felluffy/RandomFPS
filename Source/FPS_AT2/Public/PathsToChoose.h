// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathsToChoose.generated.h"

UCLASS()
class FPS_AT2_API APathsToChoose : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathsToChoose();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Patrol")
		TArray<AActor*> waypoints;

public:
	UFUNCTION(BlueprintCallable)
	inline TArray<AActor*> GetWaypoints() { return waypoints; }


};
