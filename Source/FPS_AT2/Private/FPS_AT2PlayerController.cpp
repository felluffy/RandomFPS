// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_AT2PlayerController.h"
#include "NPC_AI_Controller.h"

void AFPS_AT2PlayerController::BeginPlay()
{
	Super::BeginPlay();
	//RegisterredControllers = new TArray<ANPC_AI_Controller>();
	//UE_LOG(LogTemp, Warning, TEXT("idiot"));
}


bool AFPS_AT2PlayerController::CommandMove(FVector &WorldPosition)
{
	if (NPCs.Num() < 1)
		return false;

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
	if (NPCs.Num() < 1)
		return;
	if (RegisterredControllers.IsValidIndex(index))
		RegisterredControllers.Remove(NPCs[index]);
	else
		RegisterredControllers.Add(NPCs[index]);
}


void AFPS_AT2PlayerController::Test()
{
	//for (int i = 0; i != RegisterredControllers.Num(); i++)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s reporting"), *(RegisterredControllers[i]->GetPawn()->GetName()));
	}
	UE_LOG(LogTemp, Error, TEXT("%d reporting"), RegisterredControllers.Num());

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


void AFPS_AT2PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AFPS_AT2PlayerController::OnToggleInGameMenu);
}

void AFPS_AT2PlayerController::OnToggleInGameMenu()
{

}