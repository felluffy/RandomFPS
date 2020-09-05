// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_GameStateSP.h"
int32 AFPS_GameStateSP::AddScore_Implementation(int WhichTeamScored)
{
	switch (WhichTeamScored)
	{
	case 1:
		TeamAScore++;
		break;
	case 2:
		TeamBScore++;
		break;
	}
	return WhichTeamScored;
}

void AFPS_GameStateSP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPS_GameStateSP, TeamAScore);
	DOREPLIFETIME(AFPS_GameStateSP, TeamBScore);

	DOREPLIFETIME(AFPS_GameStateSP, bIsGameOver);
	DOREPLIFETIME(AFPS_GameStateSP, bReadyToStart);

	DOREPLIFETIME(AFPS_GameStateSP, RoundTimesInSeconds);
	DOREPLIFETIME(AFPS_GameStateSP, TimeElapsedSinceGameStart);
}

void AFPS_GameStateSP::OnStartMatch_Implementation()
{

}

void AFPS_GameStateSP::UpdateMatchTime_Implementation()
{

}

void AFPS_GameStateSP::StartMatch_Implementation()
{

}

