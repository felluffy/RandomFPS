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

void ASinglePlayerGamemodeBase::DestroyActorFunction()
{
	if (Spawned_Character)
	{
		Spawned_Character->Destroy();
	}
}

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
			AFPS_Charachter* OwnedCharachter = Cast<AFPS_Charachter>(AIC->GetPawn());
			ExistingBots++;
			if (OwnedCharachter && PlayerController)
			{
				if(OwnedCharachter->TeamNumber == Cast<AFPS_Charachter>(PlayerController->GetPawn())->TeamNumber)
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
	/*static ConstructorHelpers::FClassFinder<APawn> BotPawnOb(TEXT("/Game/Blueprints/AI/AI_Bot"));
	BotPawnClass = BotPawnOb.Class;*/
		
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

	//UWorld* World = GetWorld();
	//FTransform SpawnLocation = FTransform({}, { 0, 0, 50 }, { 0, 0, 0 }, FVector::OneVector);
	//FActorSpawnParameters SpawnParams;
	//Spawned_Character = World->SpawnActor<AFPS_Charachter>(AFPS_Charachter::StaticClass(), { 0,0,50 }, { 0,0,0 });
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Spawned_Character->GetName());
	CreateBotControllers();
	//FTimerHandle Timer;
	//GetWorldTimerManager().SetTimer(Timer, this, &ASinglePlayerGamemodeBase::DestroyActorFunction, 10);
}