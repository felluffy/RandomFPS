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
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Replicated)
		float RoundTimesInSeconds;
	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bReadyToStart = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsGameOver = 0;
	UPROPERTY(BlueprintReadWrite, Replicated)
		float TimeElapsedSinceGameStart = 0;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddScore(int WhichTeamScored);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnStartMatch();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateMatchTime();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StartMatch();
};
