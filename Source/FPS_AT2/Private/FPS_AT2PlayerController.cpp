// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_AT2PlayerController.h"
#include "NPC_AI_Controller.h"
#include "FPS_Charachter.h"
#include "NavigationSystem.h"
#include "NavFilters/RecastFilter_UseDefaultArea.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include <vector>
#include <algorithm>
#include "EngineGlobals.h"

void AFPS_AT2PlayerController::BeginPlay()
{
	Super::BeginPlay();
	//RegisterredControllers = new TArray<ANPC_AI_Controller>();
}


bool AFPS_AT2PlayerController::CommandMove_Implementation(FVector &WorldPosition, bool FromMap)
{
	
	//OLD
	//if (NPCs.Num() < 1)
	//	return false;

	//uint8 Index = rand() % NPCs.Num();
	//ANPC_AI_Controller* RandomAI = NPCs[Index];
	//if (RandomAI)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("BLABBLA %s"), *RandomAI->GetName());
	//	RandomAI->MoveToLocation(WorldPosition);
	//	return true;
	//}
	//return false;
	auto Num = RegisteredNumberOfBots();
	TArray<FVector> Locations;
	for (int i = 0, j = 0; i != RegisterredControllersPair.Num(); i++)
	{
		if (!RegisterredControllersPair[i].second)
			continue;
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), WorldPosition, 110, 4, FColor::Red, 15, 5);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Command move - world pos %f %f %f"), WorldPosition.X, WorldPosition.Y, WorldPosition.Z));
		RegisterredControllersPair[i].first->TargetPoint = WorldPosition;
		RegisterredControllersPair[i].first->IsPlayerCommanded = true;
		RegisterredControllersPair[i].first->CharacterToFollow = nullptr;
		//use to not congest
		RegisterredControllersPair[i].first->bShouldMoveOnAsked = true; 
		Locations.Push({ WorldPosition.X, WorldPosition.Y, WorldPosition.Z }), j++;
		if (Num > 1 && j > 1)
		{
			FVector SpawnVector;
			SpawnVector.X = FMath::FRandRange(50, 100);
			SpawnVector.Y = FMath::FRandRange(50, 100);
			SpawnVector.Z = FMath::FRandRange(50, 150);
			RegisterredControllersPair[i].first->TargetPoint += SpawnVector;
		}
	}
	return true;
		

}

void AFPS_AT2PlayerController::RegisterBot(int index)
{
	/*if (NPCs.Num() < 1)
		return;
	if (RegisterredControllers.IsValidIndex(index))
		RegisterredControllers.Remove(NPCs[index]);
	else
		RegisterredControllers.Add(NPCs[index]);*/

	if (RegisterredControllersPair.Num() < 1 || !RegisterredControllersPair.IsValidIndex(index) || RegisterredControllersPair[index].first == NULL)
		return;
	if (RegisterredControllersPair[index].second == true)
		RegisterredControllersPair[index].second = false, RegisteredBotsNum--, Listening[index] = false;
	else
		RegisterredControllersPair[index].second = true, RegisteredBotsNum++, Listening[index] = true;
	bool res = RegisterredControllersPair[index].second;
	FString resString;
	if (res)
		resString = "True";
	else
		resString = "False";	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Controller registered INDEX: %d, on bool: %s, registerred bots: %d"), index, *resString, RegisteredBotsNum));
}


void AFPS_AT2PlayerController::ShowEndGameOrNextRoundScreen1_Implementation(class UUserWidget* WidgetToShow, bool EndGame = false)
{
	if (WidgetToShow != nullptr)
	{
		WidgetToShow->AddToViewport();
		if (EndGame)
		{
			
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetWidgetToFocus(WidgetToShow->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			SetInputMode(InputMode);

			bShowMouseCursor = true;
			bEnableClickEvents = true;
			bEnableMouseOverEvents = true;
		}
	}
}

void AFPS_AT2PlayerController::ShowPlayerSelectionOnStartUp_Implementation()
{

}

void AFPS_AT2PlayerController::Test()
{
	//for (int i = 0; i != RegisterredControllers.Num(); i++)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s reporting"), *(RegisterredControllers[i]->GetPawn()->GetName()));
	}
	//UE_LOG(LogTemp, Error, TEXT("%d reporting"), RegisterredControllers.Num());

}

void AFPS_AT2PlayerController::OnPossess(APawn* aPawn)
{
	if (this != nullptr && this->PlayerCameraManager != nullptr)
		this->PlayerCameraManager->StopAllCameraShakes(true);

	Super::OnPossess(aPawn);
	auto FPS_Char = Cast<AFPS_Charachter>(aPawn);
	if(FPS_Char)
	{
		ShowHud();
	}
}

void AFPS_AT2PlayerController::OnUnPossess()
{
	if (this != nullptr && this->PlayerCameraManager != nullptr)
		this->PlayerCameraManager->StopAllCameraShakes(true);
	Super::OnUnPossess();
	DisableHud();
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

void AFPS_AT2PlayerController::OrderAttack_Implementation(FVector &Location)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ORder attack")));
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> ActorsOut;
	TArray<TEnumAsByte<enum EObjectTypeQuery>> objectTypes;
	objectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	objectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);
	objectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3);
	ActorsToIgnore.Add(this->GetPawn());
	for (int i = 0; i != RegisterredControllersPair.Num(); i++)
	{
		ActorsToIgnore.Add(Cast<AFPS_Charachter>(RegisterredControllersPair[i].first->GetPawn()));
	}
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), Location, { 800, 800, 200 }, objectTypes, AFPS_Charachter::StaticClass(), ActorsToIgnore, ActorsOut);

	if (ActorsOut.Num() < 1)
		return;
	
	for (int i = 0; i != RegisterredControllersPair.Num(); i++)		
	{	
		//TODO if not fighting only call them here

		if (RegisterredControllersPair[i].first->bHasLOSToEnemy)
		{
			RegisterredControllersPair[i].first->bShouldAttack = true;

			//RegisterredControllersPair[i].first->EnemyCharacterCurrentlyInFocus = Cast<AFPS_Charachter>(ActorsOut[0]);
			
		}
		/*RegisterredControllersPair[i].first->EnemyCharacterCurrentlyInFocus = Cast<AFPS_Charachter>(ActorsOut[0]);*/
		for (int j = 0; j != ActorsOut.Num(); j++)
		{
			RegisterredControllersPair[i].first->ThreatList.AddUnique(Cast<AFPS_Charachter>(ActorsOut[j]));
		}
		RegisterredControllersPair[i].first->IsPlayerCommanded = true;
	}
	return;
}

void AFPS_AT2PlayerController::OrderFollow_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Follow")));
	auto FPSPlayer = Cast<AFPS_Charachter>(this->GetPawn());
	if (FPSPlayer)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player is %s"), *FPSPlayer->GetName()));
	}
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
				//RandomPoint in navmesh if possible
				UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (navSystem)
				{
					ANavigationData* navData = navSystem->GetNavDataForProps(GetNavAgentPropertiesRef());
					TSubclassOf<UNavigationQueryFilter> filterClass = URecastFilter_UseDefaultArea::StaticClass();
					if (!filterClass || !navData)
						break;
					auto SQF = UNavigationQueryFilter::GetQueryFilter(*navData, filterClass);
					FNavLocation ResultLocation;
					if (navSystem->GetRandomReachablePointInRadius(PlayerLocation, 100, ResultLocation, navData, SQF))
					{
						PlayerLocation = ResultLocation.Location;
					}
				}

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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Guard")));
	UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if(navSystem)
	{
		ANavigationData* navData = navSystem->GetNavDataForProps(GetNavAgentPropertiesRef());
		TSubclassOf<UNavigationQueryFilter> filterClass = URecastFilter_UseDefaultArea::StaticClass();
		if(!filterClass || !navData)
			return;
		auto SQF = UNavigationQueryFilter::GetQueryFilter(*navData, filterClass);
		//bool navResult = navSystem->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportFadeTimeExtent);
		for (int i = 0; i != RegisterredControllersPair.Num(); i++)
		{
			if (!RegisterredControllersPair[i].second)
				continue;
			if (RegisterredControllersPair[i].first->bHasLOSToEnemy == false)
			{
			//bool navResult = navSystem->ProjectPointToNavigation(RegisterredControllersPair[i].first->GetPawn()->GetActorLocation(), Location, FVector::ZeroVector, navData, filterClass);
				const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
				FNavLocation outLocation;
				auto Loc = Location;
				FVector ZV = FVector::ZeroVector;
				bool navResult = navSystem->ProjectPointToNavigation(Loc, outLocation, ZV, navData, SQF);// , filterClass);
				//UE_LOG(LogTemp, Error, TEXT("%d - casted point to navigation plane"), navResult);
				if (navResult)
				{
					RegisterredControllersPair[i].first->bShouldGuard = true;
					RegisterredControllersPair[i].first->TargetPoint = Location;
					RegisterredControllersPair[i].first->IsPlayerCommanded = true;
				}
			}
		}
	}
}

void AFPS_AT2PlayerController::OrderDismiss_Implementation()
{
	for (int i = 0; i != RegisterredControllersPair.Num(); i++)
	{
		RegisterredControllersPair[i].first->IsPlayerCommanded = false;
		RegisterredControllersPair[i].first->bShouldGuard = false;
		RegisterredControllersPair[i].first->CharacterToFollow = NULL;
	}
}

void AFPS_AT2PlayerController::OrderCallAssistance_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Call assistance")));
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
			RegisterredControllersPair[it.second].first->IsPlayerCommanded = true;
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
				RegisterredControllersPair[it.second].first->IsPlayerCommanded = true;
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

///On Session leave
void AFPS_AT2PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	//// Get the OnlineSubsystem we want to work with
	//IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	//if (OnlineSub)
	//{
	//	// Get the SessionInterface from the OnlineSubsystem
	//	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	//	if (Sessions.IsValid())
	//	{
	//		// Clear the Delegate
	//		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

	//		// If it was successful, we just load another level (could be a MainMenu!)
	//		if (bWasSuccessful)
	//		{
	//			UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
	//		}
	//	}
	//}
}

void AFPS_AT2PlayerController::DisableHud()
{
	ShowHudDelegate.Broadcast(this, false);
}

void AFPS_AT2PlayerController::ShowHud()
{
	ShowHudDelegate.Broadcast(this, true);
}

void AFPS_AT2PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AFPS_AT2PlayerController::OnToggleInGameMenu);
	InputComponent->BindAction("ShowScores", IE_Pressed, this, &AFPS_AT2PlayerController::OnToggleScoreBoard);
	InputComponent->BindAction("ShowScores", IE_Released, this, &AFPS_AT2PlayerController::OnHideScoreBoard);
	//InputComponent->BindAction("Com")
}

void AFPS_AT2PlayerController::OnToggleInGameMenu_Implementation()
{

}

void AFPS_AT2PlayerController::OnToggleScoreBoard_Implementation()
{

}

void AFPS_AT2PlayerController::OnHideScoreBoard_Implementation()
{

}
