// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "FPS_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AFPS_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AFPS_GameMode();
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> BotPawnClass;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TArray<APlayerController*> AllPlayerControllers;

public:
	class ANPC_AI_Controller* CreateBot(int32 BotNum);

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
		TArray<class ANPC_AI_Controller*> BotControllers;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
		class AFPS_Charachter* Spawned_Character;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
		float TimeToReset;

	UFUNCTION()
		void DestroyActorFunction();
	//virtual void StartMatch();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetGame(float TimeToRest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay")
	void SpawnPlayers();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay")
	void SpawnCharacterWithClass(class APlayerController* Controller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay")
	void SetTeamScores();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gameplay")
	void SpawnBots();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<class AFPSPlayerStart*> PlayerStartsTeamOffense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<class AFPSPlayerStart*> PlayerStartsTeamDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		TArray<class AFPSPlayerStart*> PossibleSpawnsOffense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		TArray<class AFPSPlayerStart*> PossibleSpawnsDefense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		TArray<bool> PossibleSpawnsDefenseOccupied;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		TArray<bool> PossibleSpawnsOffenseOccupied;

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void CreateBotControllers();
	uint8 ExistingBots = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		int32 TeamOffenseScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		int32 TeamDefenseScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		int32 TeamOffenseBots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		int32 TeamDefenseBots;
};
