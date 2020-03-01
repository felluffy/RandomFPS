// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPS_Charachter.h"
#include "AI_Character.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AAI_Character : public AFPS_Charachter
{
	GENERATED_BODY()
	
public:
	AAI_Character();
	UPROPERTY(EditAnywhere, Category = Behavior)
		class UBehaviorTree* BotBehavior;
};
