// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPS_CharacterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AFPS_CharacterPlayerState : public APlayerState
{
	GENERATED_BODY()
	//virtual void Reset() override;
protected:
	UPROPERTY(Replicated)
	uint8 TeamNumber = 0;
	UPROPERTY(Replicated)
	int32 ScorePoints = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "Game stats")
	uint8 GetTeam() { return TeamNumber; }

	UFUNCTION(BlueprintCallable, Category = "Game stats")
	void SetTeam(uint8 ToSetTeamNumber) { this->TeamNumber = ToSetTeamNumber; }

	UFUNCTION(BlueprintCallable, Category = "Game stats")
	int32 GetScore() { return ScorePoints; }

	UFUNCTION(BlueprintCallable, Category = "Game stats")
	void AddToScore(int32 ToAddScorePoints) { this->ScorePoints += ToAddScorePoints; }
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game stats", Replicated)
	int32 Deaths;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game stats", Replicated)
	int32 Kills;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game stats")
	class AController* Controller;
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Game stats")
	void ScorePointsFunc(AFPS_CharacterPlayerState* Victim, int32 ToAddPoints);
	UFUNCTION(BlueprintNativeEvent, Category = "Game stats")
	void ResetScore();
	UFUNCTION(BlueprintNativeEvent, Category = "Game stats")
	void Addkills(int32 ToAddKills);
	UFUNCTION(BlueprintNativeEvent, Category = "Game stats")
	void AddDeath(int32 ToChangeScorePoints);

};