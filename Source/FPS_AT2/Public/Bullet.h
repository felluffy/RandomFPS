// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class FPS_AT2_API ABullet : public AActor
{
	GENERATED_BODY()
	
		UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class USphereComponent* CollisionComp;
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class UStaticMeshComponent* MeshComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

protected:
	virtual void BeginPlay() override;

public:
	ABullet();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }


	UFUNCTION(BlueprintCallable)
		void PlayParticle(const FHitResult& Hit);
	UFUNCTION(BlueprintCallable)
		void ApplyDecal(const FHitResult& Hit);

protected:
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class UParticleSystem* ImpactParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class UParticleSystemComponent* TrailParticleComp = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	TArray<UMaterialInstance*> BulletImpactHoles;
public:
	class AFPS_Charachter* Instigator;

public:
	float fDamage = 30.0f;
	bool bIsExplosive = false;
};
