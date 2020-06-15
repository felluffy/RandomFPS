// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


//on health change event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealhChangedSignature, UHealthComponent*, HealthComponent, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_AT2_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser );

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealhChangedSignature OnHealthChanged;

	float GetHealth() const { return CurrentHealth; }
	float GetPercentageHealth() const { return CurrentHealth / MaxHealth; }
	float GetMaxHealth() const { return MaxHealth; }

	static bool IsFriendly(AActor* ActorA, AActor* ActorB);
	void SetTeam(uint8 TeamNum) { TeamNumber = TeamNum; }
	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	uint8 TeamNumber;
	bool SpecialDamageType = false;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health Attributes")
	float MaxHealth = 100.f;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health Attributes")
	float CurrentHealth;
};


