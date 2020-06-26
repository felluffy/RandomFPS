// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_GameMode.h"
#include "NPC_AI_Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "../FPS_AT2HUD.h"
#include "FPS_Charachter.h"
#include "NPC_AI_Controller.h"
#include "FPS_AT2PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include <utility>

AFPS_GameMode::AFPS_GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/FPS_CharachterBP"));
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
	CreateBotControllers();
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

void AFPS_GameMode::CreateBotControllers()
{
	UWorld* World = GetWorld();
	AFPS_AT2PlayerController* const PlayerController = Cast<AFPS_AT2PlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	AFPS_AT2PlayerController* const PlayerController_2 = Cast<AFPS_AT2PlayerController>(UGameplayStatics::GetPlayerController(World, 1));
	for (auto it = World->GetControllerIterator(); it; it++)
	{
		ANPC_AI_Controller* AIC = Cast<ANPC_AI_Controller>(*it);
		if (AIC)
		{
			//CreateBot()
			AFPS_Charachter* OwnedCharachter = Cast<AFPS_Charachter>(AIC->GetPawn());
			ExistingBots++;
			if (OwnedCharachter && PlayerController)
			{
				if (OwnedCharachter->TeamNumber == Cast<AFPS_Charachter>(PlayerController->GetPawn())->TeamNumber)
				{
					PlayerController->NPCs.Add(AIC);
					PlayerController->RegisterredControllersPair.Add(std::make_pair(AIC, false));
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of bots: %d"), ExistingBots);
}
