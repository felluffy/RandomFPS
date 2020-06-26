// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_AT2PlayerController.h"
#include "NPC_AI_Controller.h"
#include "FPS_Charachter.h"
#include "NavigationSystem.h"
#include "NavFilters/RecastFilter_UseDefaultArea.h"
#include <vector>
#include <algorithm>
#include "EngineGlobals.h"

void AFPS_AT2PlayerController::BeginPlay()
{
	Super::BeginPlay();
	//RegisterredControllers = new TArray<ANPC_AI_Controller>();
	//UE_LOG(LogTemp, Warning, TEXT("idiot"));
}


bool AFPS_AT2PlayerController::CommandMove_Implementation(FVector &WorldPosition)
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
	/*if (NPCs.Num() < 1)
		return;
	if (RegisterredControllers.IsValidIndex(index))
		RegisterredControllers.Remove(NPCs[index]);
	else
		RegisterredControllers.Add(NPCs[index]);*/
	if (RegisterredControllersPair.Num() < 1 || !RegisterredControllersPair.IsValidIndex(index))
		return;
	if (RegisterredControllersPair[index].second == true)
		RegisterredControllersPair[index].second = false, RegisteredBotsNum--;
	else
		RegisterredControllersPair[index].second = true, RegisteredBotsNum++;
	bool res = RegisterredControllersPair[index].second;
	FString resString;
	if (res)
		resString = "True";
	else
		resString = "False";
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Controller registered INDEX: %d, on bool: %s"), index, *resString));
		
	UE_LOG(LogTemp, Error, TEXT("Controller registered INDEX: %d, on bool: %s"), index, *resString);
}


void AFPS_AT2PlayerController::Test()
{
	//for (int i = 0; i != RegisterredControllers.Num(); i++)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s reporting"), *(RegisterredControllers[i]->GetPawn()->GetName()));
	}
	UE_LOG(LogTemp, Error, TEXT("%d reporting"), RegisterredControllers.Num());

}

uint8 AFPS_AT2PlayerController::RegisteredNumberOfBots()
{
	uint8 Num = 0;
	for (int i = 0; i != RegisterredControllersPair.Num(); i++)
	{
		if (RegisterredControllersPair[i].second)
			Num++;
	}
	return Num;
}

void AFPS_AT2PlayerController::OrderAttack_Implementation()
{

}

void AFPS_AT2PlayerController::OrderFollow_Implementation()
{
	auto FPSPlayer = Cast<AFPS_Charachter>(this->GetPawn());
	//if (FPSPlayer != NULL && RegisterredControllers.Num() > 0)
	//{
	//	for (int i = 0; i != RegisterredControllersPair.Num(); i++)
	//	{
	//		auto PlayerLocation = FPSPlayer->GetActorLocation();
	//		RegisterredControllers[i]->TargetPoint = PlayerLocation;
	//		RegisterredControllers[i]->IsPlayerCommanded = true;
	//		RegisterredControllers[i]->AcceptableRadius = 500.0f;
	//		//RegisterredControllers[i]->
	//	}
	//}

	if (FPSPlayer != NULL && RegisteredBotsNum > 0)
	{
		auto PlayerLocation = FPSPlayer->GetActorLocation();
		for (int i = 0; i != RegisterredControllersPair.Num(); i++)
		{
			if(!RegisterredControllersPair[i].second)
				continue;
			else
			{
				RegisterredControllersPair[i].first->TargetPoint = PlayerLocation;
				RegisterredControllersPair[i].first->IsPlayerCommanded = true;
				RegisterredControllersPair[i].first->AcceptableRadius = AcceptanceRadius;
				RegisterredControllersPair[i].first->CharacterToFollow = FPSPlayer;
			}
		}
	}
}

void AFPS_AT2PlayerController::OrderGuard_Implementation(FVector &Location)
{
	UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	ANavigationData* navData = navSystem->GetNavDataForProps(GetNavAgentPropertiesRef());
	TSubclassOf<UNavigationQueryFilter> filterClass = URecastFilter_UseDefaultArea::StaticClass();
	auto SQF = UNavigationQueryFilter::GetQueryFilter(*navData, filterClass);
	//bool navResult = navSystem->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportFadeTimeExtent);
	for (int i = 0; i != RegisterredControllersPair.Num(); i++)
	{
		if (RegisterredControllersPair[i].first->bHasLOSToEnemy == false)
		{
			//bool navResult = navSystem->ProjectPointToNavigation(RegisterredControllersPair[i].first->GetPawn()->GetActorLocation(), Location, FVector::ZeroVector, navData, filterClass);
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			FNavLocation outLocation;
			auto Loc = Location;
			FVector ZV = FVector::ZeroVector;
			bool navResult = navSystem->ProjectPointToNavigation(Loc, outLocation, ZV, navData, SQF);// , filterClass);
			UE_LOG(LogTemp, Error, TEXT("%d - casted point to navigation plane"), navResult);
		}
	}
}

void AFPS_AT2PlayerController::OrderDismiss_Implementation()
{
	for (int i = 0; i != RegisterredControllersPair.Num(); i++)
	{
		RegisterredControllersPair[i].first->IsPlayerCommanded = false;
	}
}

void AFPS_AT2PlayerController::OrderCallAssistance_Implementation()
{
	auto FPSPlayer = Cast<AFPS_Charachter>(this->GetPawn());
	
	TArray<AFPS_Charachter*> ClosestCharacters;
	std::vector<std::pair<float, int>> distances;
	if (FPSPlayer != NULL)
	{
		auto PlayerLocation = FPSPlayer->GetActorLocation();
		for (int i = 0; i != RegisterredControllersPair.Num(); i++)
		{
			auto BotLocation = RegisterredControllersPair[i].first->GetPawn()->GetActorLocation();
			//TODO Average out the distance between the path distance and the direct distance LOOK for static UNavigationPath * FindPathToLocationSynchronously
			distances.push_back({ (PlayerLocation - BotLocation).Size(), i });
		}
	}
	std::sort(distances.begin(), distances.end(), [](std::pair<float, int> a, std::pair<float, int> b) { return a.first < b.first; });
	bool SuccessfullyCalledSomeoneInDistance = false;
	for (auto &it : distances)
	{
		if ((RegisterredControllersPair[it.second].first->bHasLOSToEnemy == false) && it.first < AcceptanceRadius)
		{
			RegisterredControllersPair[it.second].first->bShouldDefend = true;
			RegisterredControllersPair[it.second].first->CharacterToFollow = FPSPlayer;
			SuccessfullyCalledSomeoneInDistance = true;
		}
	}
	if (!SuccessfullyCalledSomeoneInDistance)
	{
		for (auto &it : distances)
		{
			if (RegisterredControllersPair[it.second].first->bHasLOSToEnemy == false)
			{
				RegisterredControllersPair[it.second].first->bShouldDefend = true;
				RegisterredControllersPair[it.second].first->CharacterToFollow = FPSPlayer;
				SuccessfullyCalledSomeoneInDistance = true;
				break;
			}
		}
	}

	return;
}

//void AFPS_AT2PlayerController::CommandMove(FVector &WorldPosition)
	//{
	//
	//}

void AFPS_AT2PlayerController::CallAssistance_Implementation()
{

}

void DefendAt(FVector &WorldPosition)
{
	
}

void AFPS_AT2PlayerController::AttackTargettedEnenmy(AFPS_Charachter* EnemyChar)
{
	if (EnemyChar == NULL)
		return;
	else
	{

	}
}


void AFPS_AT2PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AFPS_AT2PlayerController::OnToggleInGameMenu);
	//InputComponent->BindAction("Com")
}

void AFPS_AT2PlayerController::OnToggleInGameMenu()
{

}

void AFPS_AT2PlayerController::OnToggleScoreBoard()
{

}

void AFPS_AT2PlayerController::OnHideScoreBoard()
{

}
