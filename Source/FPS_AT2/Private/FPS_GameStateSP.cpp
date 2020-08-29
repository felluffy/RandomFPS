// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_GameStateSP.h"
void AFPS_GameStateSP::AddScore_Implementation(int WhichTeamScored)
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
	return;
}

void AFPS_GameStateSP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPS_GameStateSP, TeamAScore);
	DOREPLIFETIME(AFPS_GameStateSP, TeamBScore);
}

