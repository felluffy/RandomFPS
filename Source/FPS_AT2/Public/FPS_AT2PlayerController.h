// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPS_AT2PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AFPS_AT2PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	bool CommandMove(FVector &WorldPosition);
	TArray<class ANPC_AI_Controller*> NPCs;
};
