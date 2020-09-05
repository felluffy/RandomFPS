// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/BoxComponent.h"
#include "NPC_AI_Controller.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCalledForAssistance, ANPC_AI_Controller*, AI_Con, ANPC_AI_Controller*, Requester);

/**
 *
 */
USTRUCT(BlueprintType)
struct FCoverType
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cover)
		bool FreeUP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cover)
		bool FreeLeft;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cover)
		bool FreeRight;
};


UCLASS()
class FPS_AT2_API ANPC_AI_Controller : public AAIController
{
	GENERATED_BODY()
public:
	ANPC_AI_Controller();
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	virtual void SetFocalPoint(FVector NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay) override;
	virtual void BeginPlay() override;
private:
	//UPROPERTY(transient)
	//	UBlackboardComponent* BlackboardComp;

	/* Cached BT component */
	//UPROPERTY(transient)
		//class UBehaviorTreeComponent* BehaviorComp;

	//OnChange of any of the variables, select item to scrore and do whichever task
	void ScoreItems();
public:
	virtual void OnPossess(class APawn* InPawn) override;
	UFUNCTION(BlueprintCallable, Category = "Algorithms")
	void ReOrderByDistance(TArray<AActor*> Actors, bool OrderByLower = true);
	UFUNCTION(BlueprintCallable, Category = "Algorithms")
	int32 GetMaxInt(TArray<int32> IntArray);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tasks")
	void ShouldClearObjectiveOrSet(bool Set);

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
		bool bHeardAnything;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldFollow;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldGuard;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldCrouchInCover;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bEnemyAround;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		class AFPS_Charachter* CharacterToFollow;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		class AFPS_Charachter* EnemyCharacterCurrentlyInFocus;
	//Convert to priority queue later
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		TArray<AFPS_Charachter*> ThreatList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		TArray<class ADefendArea*> DefenseAreas;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		class AAI_Character* OwningPawn;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		FVector CurrentCoverLocation;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		FVector TargetPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AcceptableRadius;

	UFUNCTION(BlueprintCallable, Category = "AI")
		bool ShouldCrouchBehindCover();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cover)
	FCoverType CoverTypes;
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	int CallAssistance();
};
