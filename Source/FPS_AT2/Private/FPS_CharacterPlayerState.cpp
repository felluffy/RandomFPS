// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_CharacterPlayerState.h"
#include "Net/UnrealNetwork.h"
void AFPS_CharacterPlayerState::ScorePointsFunc_Implementation(AFPS_CharacterPlayerState* Victim, int32 Points)
{
	
}

void AFPS_CharacterPlayerState::ResetScore_Implementation()
{

}

void AFPS_CharacterPlayerState::Addkills_Implementation(int32 ToAddKills)
{

}

void AFPS_CharacterPlayerState::AddDeath_Implementation(int32 ToChangeScorePoints)
{

}


void AFPS_CharacterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPS_CharacterPlayerState, Kills);
	DOREPLIFETIME(AFPS_CharacterPlayerState, Deaths);
	DOREPLIFETIME(AFPS_CharacterPlayerState, TeamNumber);
	DOREPLIFETIME(AFPS_CharacterPlayerState, ScorePoints);
}
