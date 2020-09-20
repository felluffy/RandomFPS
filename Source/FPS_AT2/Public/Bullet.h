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
	UPROPERTY(EditDefaultsOnly, Category = Particles)
		TArray<class UParticleSystem*> ImpactParticles;
	UPROPERTY(EditDefaultsOnly, Category = Particles)
		class UParticleSystem* DefaultParticle;

	UPROPERTY(EditDefaultsOnly, Category = Particles)
	class UParticleSystemComponent* TrailParticleComp;
	UPROPERTY(EditDefaultsOnly, Category = Particles)
		TArray<class UMaterial*> BulletImpactHoles;
	UPROPERTY(EditDefaultsOnly, Category = Particles)
		class UMaterial* DefaultBulletHole;
	UPROPERTY(EditDefaultsOnly, Category = Particles)
	TArray<FVector> ScaleImpactParticles;
public:
	class AFPS_Charachter* Instigator;

public:
	float fDamage = 30.0f;
	bool bIsExplosive = false;
private:
	class UParticleSystem* FindImpactParticle(EPhysicalSurface SurfaceType);
	class UMaterialInstance* FindBulletImpactHole(EPhysicalSurface SurfaceType);
	template <typename T>
	T* GetSurfaceMat(EPhysicalSurface SurfaceType, TArray<T*> ArrayPassed, FVector &Scale, bool ChangeScale = false);
	void ChangeDamageOnBone(float &Damage, const FName &BoneName);
};

template <typename T>
T* ABullet::GetSurfaceMat(EPhysicalSurface SurfaceType, TArray<T*> ArrayPassed, FVector &Scale, bool ChangeScale)
{
	T* ToReturnItem = NULL;
	Scale = FVector(1, 1, 1);
	switch (SurfaceType)
	{
	case SurfaceType1:
		ToReturnItem = ArrayPassed.IsValidIndex(0) ? ArrayPassed[0] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(0) ? ScaleImpactParticles[0] : FVector(1, 1, 1);
		break;
	case SurfaceType2:
		ToReturnItem = ArrayPassed.IsValidIndex(1) ? ArrayPassed[1] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(1) ? ScaleImpactParticles[1] : FVector(1, 1, 1);
		break;
	case SurfaceType3:
		ToReturnItem = ArrayPassed.IsValidIndex(2) ? ArrayPassed[2] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(2) ? ScaleImpactParticles[2] : FVector(1, 1, 1);
		break;
	case SurfaceType4:
		ToReturnItem = ArrayPassed.IsValidIndex(3) ? ArrayPassed[3] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(3) ? ScaleImpactParticles[3] : FVector(1, 1, 1);
		break;
	case SurfaceType5:
		ToReturnItem = ArrayPassed.IsValidIndex(4) ? ArrayPassed[4] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(4) ? ScaleImpactParticles[4] : FVector(1, 1, 1);
		break;
	case SurfaceType6:
		ToReturnItem = ArrayPassed.IsValidIndex(5) ? ArrayPassed[5] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(5) ? ScaleImpactParticles[5] : FVector(1, 1, 1);
		break;
	case SurfaceType7:
		ToReturnItem = ArrayPassed.IsValidIndex(6) ? ArrayPassed[6] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(6) ? ScaleImpactParticles[6] : FVector(1, 1, 1);
		break;
	case SurfaceType8:
		ToReturnItem = ArrayPassed.IsValidIndex(7) ? ArrayPassed[7] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(7) ? ScaleImpactParticles[7] : FVector(1, 1, 1);
		break;
	case SurfaceType9:
		ToReturnItem = ArrayPassed.IsValidIndex(8) ? ArrayPassed[8] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(8) ? ScaleImpactParticles[8] : FVector(1, 1, 1);
		break;
	case SurfaceType10:
		ToReturnItem = ArrayPassed.IsValidIndex(9) ? ArrayPassed[9	] : nullptr;
		Scale = ChangeScale && ScaleImpactParticles.IsValidIndex(9) ? ScaleImpactParticles[9] : FVector(1,1,1);
		break;
	default:
		break;
	}
	return ToReturnItem;
}
