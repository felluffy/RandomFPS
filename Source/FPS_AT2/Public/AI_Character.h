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
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPatrolComponent* PatrolComp;	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
		class UBoxComponent* AreaContextBox;

protected:
	virtual void BeginPlay() override;
	
public:
	AAI_Character();
	UPROPERTY(EditAnywhere, Category = Behavior)
		class UBehaviorTree* BotBehavior;

	virtual void OnHealthChanged(class UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
	void GetPerceptionLocationAndRotation(FVector& Location, FRotator& Rotation) const;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	TArray<class ANPC_AI_Controller*> TeamMembers;
protected:
	UPROPERTY(BlueprintReadWrite, Category = DamageSense)
	bool IsDamagedWithinSeconds;


	void RotateHeadOnGuard();
private:
	class ANPC_AI_Controller* AAI_Controller;
public:
	void SetAAIController(class ANPC_AI_Controller* ToSet)
	{
		if(AAI_Controller == nullptr)
			AAI_Controller = ToSet;
	}
	class ANPC_AI_Controller* GetAAIController() { return AAI_Controller; }
	//class ANPC_AI_Controller GetController()
};
