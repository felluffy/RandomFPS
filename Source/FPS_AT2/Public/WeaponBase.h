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
	class USkeletalMeshComponent* GetFP_Gun() { return FP_Gun; }
	class USkeletalMeshComponent* GetTP_Gun() { return TP_Gun; }

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* TP_Gun;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick (float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Properties")
		void OnFire();
	UFUNCTION(BlueprintCallable, Category = "Properties")
		void WeaponFire();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FName WeaponName;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FName GripHandle = TEXT("GripHandle");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FName RootBoneName = TEXT("");

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

	FTimerHandle TimerHandle_fIntervalShootingTime, TimerHandle_fReloadTime, TimerHandle_fRecoilTime;
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
	void ReloadWeapon();
	void OnLeaveInventory();
	void SetCurrentAmmo(int32 CurrentAmmo, int32 CurrentAmmoInMagazine);
	int32 GetCurrentAmmo() { return CurrentAmmo; };
	int32 GetCurrentAmmoInMagazine() { return CurrentAmmoInMagazine; };
	//Which to fire from, FP, or 3P
	bool IsFirstPerson = true;

	void DroppedOnWorld();

	void DropOnWorld();

	protected:
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
	int32 MaxAmmo;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
	int32 MagazineAmmoCapacity;
	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmoInMagazine;
	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmo;
	bool CanFire();
	bool bEquipped = false;
	//FName ShellExitSocket = TEXT("ShellExit");

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float RandomRecoilYawHigh;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float RandomRecoilPitchHigh;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float RandomRecoilYawLow;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float RandomRecoilPitchLow;
	
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float Spread;
	
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float ReloadTime;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float RecoilTimeModifier;
	
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	float WeaponDamage;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	bool IsExplosive = false;

protected:
	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* ReloadSound;

private:
	void OnRecoil();
	float CurrentRecYaw, CurrentRecPitch;
	bool bRecoil = false;
	bool bAddedOffset = false;
};