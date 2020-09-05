// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplosiveItems.h"
#include "Components/PointLightComponent.h"
#include "GrenadeExp.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API AGrenadeExp : public AExplosiveItems
{
	GENERATED_BODY()
public:	
	AGrenadeExp();
	virtual void BeginPlay() override;
	virtual void Explode() override;
	UFUNCTION(BlueprintCallable)
	void Throw();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* PointLight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;
private:
	UPROPERTY(EditDefaultsOnly, Category = Properties)
	float FuseTime = 3;
	void DisableLight();
	FTimerHandle FuseHandle;
	FTimerHandle LightHandle;
	template <typename T>
	T* GetSurfaceMat(EPhysicalSurface SurfaceType, TArray<T*> ArrayPassed);
	class UParticleSystem* FindImpactParticle(EPhysicalSurface SurfaceType);
	class UMaterialInstance* FindBulletImpactHole(EPhysicalSurface SurfaceType);
protected:
	UFUNCTION(BlueprintCallable)
		void ApplyParticle(const FHitResult& Hit);
	UFUNCTION(BlueprintCallable)
		void ApplyDecal(const FHitResult& Hit);
protected:
	UPROPERTY(EditDefaultsOnly, Category = Particles)
		TArray<class UParticleSystem*> ImpactParticles;
	UPROPERTY(EditDefaultsOnly, Category = Particles)
		TArray<class UMaterial*> ImpactDecals;
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	class UParticleSystem* ImpactParticle;
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	class UMaterial* ImpactDecal;
public:
	UFUNCTION(BlueprintNativeEvent, Category = Defaults)
	void SetProjectileVelocityOnThrow(FVector AnyImpulse);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Defaults)
		void SetProjectileVelocityOnThrow_2(FVector AnyImpulse, float Multiplier);

};


template <typename T>
T* AGrenadeExp::GetSurfaceMat(EPhysicalSurface SurfaceType, TArray<T*> ArrayPassed)
{
	T* ToReturnItem = NULL;
	switch (SurfaceType)
	{
	case SurfaceType1:
		ToReturnItem = ArrayPassed.IsValidIndex(0) ? ArrayPassed[0] : nullptr;
		break;
	case SurfaceType2:
		ToReturnItem = ArrayPassed.IsValidIndex(1) ? ArrayPassed[1] : nullptr;
		break;
	case SurfaceType3:
		ToReturnItem = ArrayPassed.IsValidIndex(2) ? ArrayPassed[2] : nullptr;
		break;
	case SurfaceType4:
		ToReturnItem = ArrayPassed.IsValidIndex(3) ? ArrayPassed[3] : nullptr;
		break;
	case SurfaceType5:
		ToReturnItem = ArrayPassed.IsValidIndex(4) ? ArrayPassed[4] : nullptr;
		break;
	case SurfaceType6:
		ToReturnItem = ArrayPassed.IsValidIndex(5) ? ArrayPassed[5] : nullptr;
		break;
	case SurfaceType7:
		ToReturnItem = ArrayPassed.IsValidIndex(6) ? ArrayPassed[6] : nullptr;
		break;
	case SurfaceType8:
		ToReturnItem = ArrayPassed.IsValidIndex(7) ? ArrayPassed[7] : nullptr;
		break;
	case SurfaceType9:
		ToReturnItem = ArrayPassed.IsValidIndex(8) ? ArrayPassed[8] : nullptr;
		break;
	case SurfaceType10:
		ToReturnItem = ArrayPassed.IsValidIndex(9) ? ArrayPassed[9] : nullptr;
		break;
	default:
		break;
	}
	if(ToReturnItem == NULL)
		ToReturnItem = ArrayPassed.IsValidIndex(0) ? ArrayPassed[0] : nullptr;
	return ToReturnItem;
}
