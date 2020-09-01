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


USTRUCT(BlueprintType)
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
		class UAnimMontage	* Pawn1P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
		class UAnimMontage* Pawn3P;
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
		FWeaponAnim FireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FWeaponAnim FireAnimationSights;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FWeaponAnim ReloadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FWeaponAnim ReloadAnimationSights;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FWeaponAnim EquipAnimation;

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

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float FireSoundScale = .2f;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float FireSoundRange = 2000.0f;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float ReloadSoundScale = .2f;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float ReloadSoundRange = 2000.0f;
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float WeaponFOV = 90.0f;

	FTimerHandle TimerHandle_fIntervalShootingTime, TimerHandle_fReloadTime, TimerHandle_fRecoilTime;
	void GiveAmmo(int AddAmount);

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
		float fWeaponRange = 5000;
public:
	float GetWeaponFov() { return WeaponFOV; }
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
	int32 GetMagazineAmmoCapacity() { return MagazineAmmoCapacity; }
	//Which to fire from, FP, or 3P
	bool IsFirstPerson = true;

	void DroppedOnWorld();

	void DropOnWorld();
	bool bZoomed = false;
	protected:
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Classifiers)
	int32 MaxAmmo;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Weapon_Classifiers)
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

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
		bool ApplyBothAnimations = false;

	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	FName ZoomSocketName = "RifleZoomSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon_Properties)
	FName MagazineSocket = "MagazineSocket";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon_Properties)
	FName MagazineAttachSocket = "MagazineSocket";
	UPROPERTY(EditDefaultsOnly, Category = Weapon_Properties)
	FWeaponAnim Animations;
	

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* ZoomCameraComponent;
private:
	float PlayWeaponAnimation(const FWeaponAnim& Animation, float InPlayRate = 1.F);
	void StopWeaponAnimation(const FWeaponAnim& Animation);


protected:
	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Transient)
	class USoundBase* EmptyClip;
	
	UPROPERTY(EditDefaultsOnly, Transient)
	USoundAttenuation* EmptyClipAttenuation;
	UPROPERTY(EditDefaultsOnly, Transient)
	USoundAttenuation* FireSoundAttenuation;
	UPROPERTY(EditDefaultsOnly, Transient)
	USoundAttenuation* ReloadSoundAttenuation;

private:
	void OnRecoil();
	float CurrentRecYaw, CurrentRecPitch;
	bool bRecoil = false;
	bool bAddedOffset = false;
	float FireShakeAlpha = 1.0f;
	bool bIsReloading;
	FRandomStream RS;
	bool bShouldAddRecoilAI;

public:
	UFUNCTION(BlueprintCallable, Category = "WeaponProperties")
	bool IsReloading() { return bIsReloading; }

//private:
//	//*********
//	// Rotation Lag/Lead
//
//	/** Previous frame's weapon rotation */
//	UPROPERTY()
//		FRotator LastRotation;
//
//	/** Saved values used for lagging weapon rotation */
//	UPROPERTY()
//		float	OldRotDiff[2];
//	UPROPERTY()
//		float	OldLeadMag[2];
//	UPROPERTY()
//		float	OldMaxDiff[2];
//
//	/** How fast Weapon Rotation offsets */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Rotation")
//		float	RotChgSpeed;
//
//	/** How fast Weapon Rotation returns */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Rotation")
//		float	ReturnChgSpeed;
//
//	/** Max Weapon Rotation Yaw offset */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Rotation")
//		float	MaxYawLag;
//
//	/** Max Weapon Rotation Pitch offset */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Rotation")
//		float	MaxPitchLag;
//
//	/** If true, the weapon's rotation will procedurally lag behind the holder's rotation */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Rotation")
//		bool bProceduralLagRotation;
//
//	/** @return whether the weapon's rotation is allowed to lag behind the holder's rotation */
//	virtual bool ShouldLagRot();
//
//	/** Lag a component of weapon rotation behind player's rotation. */
//	virtual float LagWeaponRotation(float NewValue, float LastValue, float DeltaTime, float MaxDiff, int32 Index);
//
//	/** called when initially attaching the first person weapon and when a camera viewing this player changes the weapon hand setting */
//	virtual void UpdateWeaponHand();
};