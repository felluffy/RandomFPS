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
	//UPROPERTY(transient)
	//	UBlackboardComponent* BlackboardComp;

	/* Cached BT component */
	//UPROPERTY(transient)
		//class UBehaviorTreeComponent* BehaviorComp;
protected: 
		virtual void OnPossess(class APawn* InPawn) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool IsPlayerCommanded;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bAskedAssistance;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bHasLOSToEnemy;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bHadLOSToEnemyBack;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bShouldSwapWeapons;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bShouldReload;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bShouldDefend;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldAttack;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	bool bIsSuspicious;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldFollow;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	class AFPS_Charachter* CharacterToFollow;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FVector TargetPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float AcceptableRadius;
	

	void CallAssistance();
};
