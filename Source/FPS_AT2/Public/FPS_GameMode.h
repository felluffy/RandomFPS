// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FPS_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AFPS_GameMode : public AGameMode
{
public:
	AFPS_GameMode();
	virtual void BeginPlay() override;

	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> BotPawnClass;

	class ANPC_AI_Controller* CreateBot(int32 BotNum);

	UPROPERTY()
		TArray<class ANPC_AI_Controller*> BotControllers;

	UPROPERTY()
		class AFPS_Charachter* Spawned_Character;

	UFUNCTION()
		void DestroyActorFunction();
	//virtual void StartMatch();

public:
	void CreateBotControllers();
	uint8 ExistingBots = 0;
};
