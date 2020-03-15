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
private:
	//uint8 TeamNumber = 0;
	//uint8 ScorePoints = 0;
public:
	//uint8 GetTeam() { return TeamNumber; }
	//void SetTeam(uint8 TeamNumber) { this->TeamNumber = TeamNumber; }
	//uint8 GetScore() { return ScorePoints; }
	//void AddToScore(uint8 Score) { ScorePoints += ScorePoints; }
public :
	void ScoreKill(AFPS_CharacterPlayerState* Victim, int32 Points);
	//void BroadcastDeath(class AFPS_CharacterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPS_CharacterPlayerState* KilledPlayerState);
	
};
