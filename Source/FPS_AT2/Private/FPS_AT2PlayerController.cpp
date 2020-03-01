// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_AT2PlayerController.h"
#include "NPC_AI_Controller.h"

void AFPS_AT2PlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("idiot"));
}


bool AFPS_AT2PlayerController::CommandMove(FVector &WorldPosition)
{
	uint8 Index = rand() % NPCs.Num();
	ANPC_AI_Controller* RandomAI = NPCs[Index];
	if (RandomAI)
	{
		UE_LOG(LogTemp, Warning, TEXT("BLABBLA %s"), *RandomAI->GetName());
		RandomAI->MoveToLocation(WorldPosition);
		return true;
	}
	return false;
}