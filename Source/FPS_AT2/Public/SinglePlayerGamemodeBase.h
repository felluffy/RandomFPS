// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SinglePlayerGamemodeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API ASinglePlayerGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASinglePlayerGamemodeBase();
public:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
		TSubclassOf<APawn> BotPawnClass;

	class ANPC_AI_Controller* CreateBot(int32 BotNum);

	UPROPERTY()
		TArray<class ANPC_AI_Controller*> BotControllers;

	//virtual void StartMatch();

public:
	void CreateBotControllers();
	uint8 ExistingBots = 0;
};
