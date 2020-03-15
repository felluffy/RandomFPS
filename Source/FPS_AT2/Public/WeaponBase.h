// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM()
enum class EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};


UCLASS()
class FPS_AT2_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* TP_Gun;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Properties")
		void OnFire();
	UFUNCTION(BlueprintCallable, Category = "Properties")
		void WeaponFire();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class ABullet> ProjectileClass;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;*/

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ReloadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* EquipAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<class UCameraShake> FireShake;
	inline void SetFiring(bool state) { bAllowedToFire = state; }

	virtual void StartFire();
	virtual void StopFire();

private:
	bool bAllowedToFire = false;
	float fLastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float fIntervalShootingTime = .2f;

	FTimerHandle TimerHandle_fIntervalShootingTime, TimerHandle_fReloadTime;
	void GiveAmmo(int AddAmount);

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float fWeaponRange = 5000;
public:
	void SetOwningPawn(class AFPS_Charachter* NewOwner);
	void AttachMeshToPawn();
	
	class AFPS_Charachter* Owner = nullptr;
	void Unequip();
	void Equip();
	void Reload();
	void OnLeaveInventory();

	//Which to fire from, FP, or 3P
	bool IsFirstPerson = true;

	void DroppedOnWorld();

private:
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
	int16 MaxAmmo;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
	int16 MagazineAmmoCapacity;
	int16 CurrentAmmoInMagazine;
	int16 CurrentAmmo;
	bool CanFire();
	bool bEquipped = false;
	//FName ShellExitSocket = TEXT("ShellExit");

protected:
	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* ReloadSound;



};
