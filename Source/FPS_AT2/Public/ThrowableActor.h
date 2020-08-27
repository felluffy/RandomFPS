// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableActor.generated.h"

UCLASS()
class FPS_AT2_API AThrowableActor : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class URadialForceComponent* RadialForceComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Particles)
		TArray<class UParticleSystem*> ExplosionParticles;

	
public:	
	// Sets default values for this actor's properties
	AThrowableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float FuseTime;
	UPROPERTY(VisibleDefaultsOnly, Category = Misc)
		float ExplosionRadius;
	UPROPERTY(VisibleDefaultsOnly, Category = Misc)
		float Damage;
private:
	FTimerHandle FuseTimerHandle;
};
