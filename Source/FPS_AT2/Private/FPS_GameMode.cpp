// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_GameMode.h"
#include "NPC_AI_Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "../FPS_AT2HUD.h"
#include "FPS_Charachter.h"
#include "NPC_AI_Controller.h"
#include "FPS_AT2PlayerController.h"
#include "AI_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include <utility>

AFPS_GameMode::AFPS_GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/FPS_CharachterBP"));
	//static ConstructorHelpers::FClassFinder<AController> ControllerClassFinder(TEXT("/Game/Blueprints/"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	/*static ConstructorHelpers::FClassFinder<APawn> BotPawnOb(TEXT("/Game/Blueprints/AI/AI_Bot"));
	BotPawnClass = BotPawnOb.Class;*/

	// use our custom HUD class
	HUDClass = AFPS_AT2HUD::StaticClass();
	PlayerControllerClass = AFPS_AT2PlayerController::StaticClass();
}

void AFPS_GameMode::BeginPlay()
{
	Super::BeginPlay();
	//CreateBotControllers();
	//RegisterAllToDelegates();
}

ANPC_AI_Controller* AFPS_GameMode::CreateBot(int32 BotNum)
{
	return nullptr;
}

void AFPS_GameMode::DestroyActorFunction()
{
	if (Spawned_Character)
	{
		Spawned_Character->Destroy();
	}
}

void AFPS_GameMode::SetTeamScores_Implementation()
{

}

void AFPS_GameMode::SpawnCharacterWithClass_Implementation(class APlayerController* Controller)
{

}
//bool AFPS_GameMode::SpawnCharacterWithClass_Validate(class APlayerController* Controller)
//{
//	return true;
//}

void AFPS_GameMode::SpawnPlayers_Implementation()
{

}

void AFPS_GameMode::ResetGame_Implementation(float TimeToRest)
{

}

void AFPS_GameMode::SpawnBots_Implementation()
{

}

void AFPS_GameMode::CreateBotControllers()
{
	UWorld* World = GetWorld();
	AFPS_AT2PlayerController* const PlayerController_1 = Cast<AFPS_AT2PlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	AFPS_AT2PlayerController* const PlayerController_2 = Cast<AFPS_AT2PlayerController>(UGameplayStatics::GetPlayerController(World, 1));
	AFPS_Charachter* TeamALeader = NULL;
	AFPS_Charachter* TeamBLeader = NULL;
	TArray<ANPC_AI_Controller*> Team1Players;
	TArray<ANPC_AI_Controller*> Team2Players;

	if (PlayerController_1)
	{
		TeamALeader = Cast<AFPS_Charachter>(PlayerController_1->GetPawn());
		PlayerController_1->RegisterredControllersPair.Empty();
	}
	if (PlayerController_2)
	{
		TeamBLeader = Cast<AFPS_Charachter>(PlayerController_2->GetPawn());
		PlayerController_2->RegisterredControllersPair.Empty();
	}
	
	for (auto it = World->GetControllerIterator(); it; it++)
	{
		ANPC_AI_Controller* AIC = Cast<ANPC_AI_Controller>(*it);
		if (AIC)
		{
			//CreateBot()
			AFPS_Charachter* OwnedCharachter = Cast<AFPS_Charachter>(AIC->GetPawn());
			
			ExistingBots++;
			if (OwnedCharachter && PlayerController_1)
			{
				AAI_Character* Ai = Cast<AAI_Character>(AIC->GetPawn());
				if (TeamALeader != NULL && OwnedCharachter->TeamNumber == TeamALeader->TeamNumber)
				{
					
					if(Ai)
						PlayerController_1->AIChars.Add(Ai);
					PlayerController_1->RegisterredControllersPair.Add(std::make_pair(AIC, false));
					PlayerController_1->Listening.Add(false);
				}
				else if (TeamBLeader != NULL && OwnedCharachter->TeamNumber == TeamBLeader->TeamNumber)
				{
					if (Ai)
						PlayerController_2->AIChars.Add(Ai);
					PlayerController_2->RegisterredControllersPair.Add(std::make_pair(AIC, false));
					PlayerController_2->Listening.Add(false);
				}
			OwnedCharachter->TeamNumber == 1 ? Team1Players.Add(AIC) : Team2Players.Add(AIC);
			}
		}
		for (int i = 0; i != Team1Players.Num(); i++)
		{
			for (int j = 0; j != Team1Players.Num(); j++)
			{
				if (i != j)
				{
					Team1Players[i]->RegisterredControllersPair.AddUnique(std::make_pair(Team1Players[j], true));
				}
			}
		}

		for (int i = 0; i != Team2Players.Num(); i++)
		{
			for (int j = 0; j != Team2Players.Num(); j++)
			{
				if (i != j)
				{
					Team2Players[i]->RegisterredControllersPair.AddUnique(std::make_pair(Team2Players[j], true));
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of bots: %d"), ExistingBots);
}
