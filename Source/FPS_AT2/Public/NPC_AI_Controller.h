// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPC_AI_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API ANPC_AI_Controller : public AAIController
{
	GENERATED_BODY()
public:
	ANPC_AI_Controller();
private:
	UPROPERTY(transient)
		UBlackboardComponent* BlackboardComp;

	/* Cached BT component */
	UPROPERTY(transient)
		class UBehaviorTreeComponent* BehaviorComp;
protected: 
		virtual void OnPossess(class APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	bool IsPlayerCommanded;
	UPROPERTY(EditAnywhere, Category = "AI")
	bool NeedAssistance;
	UPROPERTY(EditAnywhere, Category = "AI")
	bool HasLOSToEnemy;
	UPROPERTY(EditAnywhere, Category = "AI")
	bool ShouldSwapWeapons;
	void CallAssistance();
};
