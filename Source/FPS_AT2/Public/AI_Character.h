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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPatrolComponent* PatrolComp;

protected:
	virtual void BeginPlay() override;
	
public:
	AAI_Character();
	UPROPERTY(EditAnywhere, Category = Behavior)
		class UBehaviorTree* BotBehavior;

	virtual void OnHealthChanged(class UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;
protected:
	UPROPERTY(BlueprintReadWrite, Category = DamageSense)
	bool IsDamagedWithinSeconds;

	
};
