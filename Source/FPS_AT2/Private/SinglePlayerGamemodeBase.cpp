// Fill out your copyright notice in the Description page of Project Settings.


#include "SinglePlayerGamemodeBase.h"
#include "NPC_AI_Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "../FPS_AT2HUD.h"
#include "FPS_Charachter.h"
#include "NPC_AI_Controller.h"
#include "FPS_AT2PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ASinglePlayerGamemodeBase::CreateBotControllers()
{
	UWorld* World = GetWorld();
	AFPS_AT2PlayerController* const PlayerController = Cast<AFPS_AT2PlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	for (auto it = World->GetControllerIterator(); it; it++)
	{
		ANPC_AI_Controller* AIC = Cast<ANPC_AI_Controller>(*it);
		if(AIC)
		{
			//CreateBot()
			ExistingBots++;
			if (PlayerController)
			{
				PlayerController->NPCs.Add(AIC);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of bots: %d"), ExistingBots);
}

ASinglePlayerGamemodeBase::ASinglePlayerGamemodeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/FPS_CharachterBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	static ConstructorHelpers::FClassFinder<APawn> BotPawnOb(TEXT("/Game/Blueprints/AI/AI_Bot"));
	BotPawnClass = BotPawnOb.Class;

	// use our custom HUD class
	HUDClass = AFPS_AT2HUD::StaticClass();
	PlayerControllerClass = AFPS_AT2PlayerController::StaticClass();
}


ANPC_AI_Controller* ASinglePlayerGamemodeBase::CreateBot(int32 BotNum)
{
	return nullptr;
}


void ASinglePlayerGamemodeBase::BeginPlay()
{
	Super::BeginPlay();
	CreateBotControllers();
}