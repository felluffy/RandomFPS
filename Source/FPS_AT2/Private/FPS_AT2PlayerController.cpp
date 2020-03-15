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

void AFPS_AT2PlayerController::RegisterBot(int index)
{
	if (RegisterredControllers.IsValidIndex(index))
		RegisterredControllers.Remove(NPCs[index]);
	else
		RegisterredControllers.Add(NPCs[index]);
}


	//void AFPS_AT2PlayerController::CommandMove(FVector &WorldPosition)
	//{
	//
	//}

void AFPS_AT2PlayerController::CallAssistance()
{

}

void DefendAt(FVector &WorldPosition)
{

}

void AFPS_AT2PlayerController::AttackTargettedEnenmy(AFPS_Charachter* EnemyChar)
{

}

