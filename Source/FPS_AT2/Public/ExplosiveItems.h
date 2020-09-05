// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h" 	
#include "ExplosiveItems.generated.h"

UCLASS()
class FPS_AT2_API AExplosiveItems : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveItems();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly)
	float Lifespan = 2.0f;

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;


	/*UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USphereComponent* Sphere;*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* HealthComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class URadialForceComponent* RadialForceComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particles)
		class UParticleSystem* ExplosionParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Shakes")
		TSubclassOf<class UCameraShake> OnDamagedShake;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* MeshEx_2;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	 class USceneComponent* sc;

	virtual	void Explode();

	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(class UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	public:
	FTimerHandle ExplosionTimerHandle;
	float DestroyComponenetDelay;
	bool bIsDestroyed = false;
	UPROPERTY(EditDefaultsOnly, Category = Misc)
		float ExplosionRadius;
	UPROPERTY(EditDefaultsOnly, Category = Misc)
		float Damage;
	UPROPERTY(EditDefaultsOnly, Transient)
		class USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float ExplosionSoundScale = .2f;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float ExplosionSoundRange = 2000.0f;
	UPROPERTY(EditDefaultsOnly, Transient)
		class USoundAttenuation* ExplosionAttenuation;
	//@TODO Move out later
public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AISenses)
	//	class UAIPerceptionSystem* PerceptionSystem;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AISenses)
	//	class UAISense_Hearing* HearingPerception;
};