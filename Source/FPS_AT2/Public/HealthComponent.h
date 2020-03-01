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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealhChangedSignature OnHealthChanged;

	float GetHealth() const { return CurrentHealth; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health Attributes")
	float MaxHealth = 100.f;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health Attributes")
	float CurrentHealth;
};
