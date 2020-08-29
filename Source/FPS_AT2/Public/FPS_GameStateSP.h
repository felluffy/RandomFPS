// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"
#include "FPS_GameStateSP.generated.h"


/**
 * 
 */
UCLASS()
class FPS_AT2_API AFPS_GameStateSP : public AGameState
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
		int32 TeamAScore;
	UPROPERTY(BlueprintReadOnly, Replicated)
		int32 TeamBScore;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddScore(int WhichTeamScored);
};
