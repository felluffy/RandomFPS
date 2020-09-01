// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <utility>  
#include "FPS_AT2PlayerController.generated.h"

/**
 * 
 */

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(CallAssistance, UHealthComponent*, HealthComponent, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(CallAssistace)

UCLASS()
class FPS_AT2_API AFPS_AT2PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite)
	uint8 TeamNumber;
public:
	virtual void SetupInputComponent() override;

	//UPROPERTY(BlueprintAssignable, Category = "Events")
	//	FOnCallAssistance CallAssistance_NPCs;
	
	void DefendAt(FVector &WorldPosition);
	void AttackTargettedEnenmy(class AFPS_Charachter* EnemyChar);
	void RegisterBot(int index);
	//void 
	//void 

	TArray<class ANPC_AI_Controller*> RegisterredControllers, NPCs;

	UPROPERTY(BlueprintReadOnly)
	TArray<class AAI_Character*> AIChars;
	//Either Activated or not

	TArray<std::pair<class ANPC_AI_Controller*, bool>> RegisterredControllersPair;
	UPROPERTY(BlueprintReadOnly, Category = AI)
	TArray<bool> Listening;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Menus")
	void OnToggleInGameMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Menus")
	void OnToggleScoreBoard();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Menus")
	void OnHideScoreBoard();
	void Test();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Settings")
	float AcceptanceRadius = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Settings")
		bool bIsInGame;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	void OrderAttack(FVector &Location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	void OrderFollow();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	void OrderGuard(FVector &Location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	void OrderDismiss();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	void OrderCallAssistance();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	bool CommandMove(FVector &WorldPosition, bool FromMap);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI_Tasks")
	void CallAssistance();

	uint8 RegisteredNumberOfBots();
private:
	uint8 RegisteredBotsNum;
};
