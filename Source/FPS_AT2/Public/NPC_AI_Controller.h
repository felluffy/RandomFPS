// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <utility>  
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
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ScoreItems();
public:
	virtual void OnPossess(class APawn* InPawn) override;
	UFUNCTION(BlueprintCallable, Category = "Algorithms")
	void ReOrderByDistance(TArray<AActor*> Actors, bool OrderByLower = true);
	UFUNCTION(BlueprintCallable, Category = "Algorithms")
	int32 GetMaxInt(TArray<int32> IntArray, int32 &RefIndex);
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
		bool bShouldFindNewCover;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldGuard;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldCrouchInCover;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bEnemyAround;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bBeingAttacked;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		bool bShouldMoveOnAsked;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		class AFPS_Charachter* CharacterToFollow;
	UPROPERTY(BlueprintReadWrite, Category = "AI")
		class AFPS_Charachter* LastCharacterFollowed;
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

	TArray<std::pair<class ANPC_AI_Controller*, bool>> RegisterredControllersPair;
	UPROPERTY(BlueprintReadOnly, Category = AI)
		TArray<bool> Listening;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		FVector TargetPoint;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AcceptableRadius;

	UFUNCTION(BlueprintCallable, Category = "AI")
		bool ShouldCrouchBehindCover();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Settings")
		float AcceptanceRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Cover)
	FCoverType CoverTypes;	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Properties")
	void DisableBehaviorTreeAndBlackboard();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Properties")
	void InitOnMatchStart();
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	int CallAssistance();
	UFUNCTION(BlueprintCallable, Category = "AI")
	void OnTimePassedClearTimer(float Seconds, float RandomizationLimit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ClearCoverAfterSet();

private:
	FTimerHandle ClearCoverTImer;
	UPROPERTY(EditDefaultsOnly)
	float TimeToClearTimerCover = 5;
	void ClearCover();

private:
	FTimerHandle ClearPlayerTimer;
	void ClearPlayer();
};
