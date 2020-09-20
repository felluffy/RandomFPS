// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "FPSPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AFPSPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team options")
	bool bIsTeamOffenseStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team options")
	bool bIsTeamStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team options")
	bool bCanBeUsed = true; //Initially true

};
