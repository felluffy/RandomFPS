// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPS_Charachter.generated.h"

UCLASS()
class FPS_AT2_API AFPS_Charachter : public ACharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;

public:
	// Sets default values for this character's properties
	AFPS_Charachter();

	USkeletalMeshComponent* GetMeshComp(bool Which)
	{
		return Which == true ? Mesh1P : Mesh3P;
	}

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
 * Called via input to turn at a given rate.
 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void BeginCrouch();
	void EndCrouch();
	void Jump() override;
	void StopJumping() override;
	void Suicide();
	void StartSprinting();
	void StopSprinting();
	void CommandBot();
	void CommandBot_1();
	void CommandBot_2();
	void CommandBot_3();
	void CommandBot_4();

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* HealthComp;
	UFUNCTION(BlueprintCallable)
		virtual void OnHealthChanged(class UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UPROPERTY(BlueprintReadOnly, Category = "Health")
		bool bInactive = false;
	UPROPERTY(EditDefaultsOnly, Category = Weapons)
		FName WeaponAttachPoint = TEXT("GripPoint");

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
		bool IsSprinting = false;
	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
		bool IsFiring = false;
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
		bool IsProning = false;
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
		bool IsCrouching = false;




protected:

	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	TArray<TSubclassOf<class AWeaponBase>> DefaultWeaponClasses;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TArray<class AWeaponBase*> Inventory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapons)
	class AWeaponBase* CurrentWeapon;
	class AWeaponBase* GetInventoryWeapon(int index) const;
	void EquipWeapon(class AWeaponBase* Weapon);
	void SetCurrentWeapon(class AWeaponBase* Weapon, class AWeaponBase* NewWeapon);
	void DropWeapon(class AWeaponBase* Weapon);
	void OnDropWeapon();
	void RemoveWeapon(class AWeaponBase* Weapon);
	void AddWeapon(class AWeaponBase* Weapon);
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	float DropWeaponMaxDistance;
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void StopFire();
	void NextWeapon();
	void PreviousWeapon();
	bool CanFire();

	bool isReloading = false; //should deactivate on a broadcast from weappon 

public: 
	//UPROPERTY(BlueprintCallable, Category = "Weapons")
	//	AWeaponBase* GetCurrentWeapon() { return CurrentWeapon; }


private:
	float LastNoiseTime;
	float SprintingSpeedModifier;
	UCharacterMovementComponent* MovementComponent;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animations")
		bool bShouldSprint = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animations")
		bool bIsJumpingToggled = false;
	bool bAllowedToFire = true;

	TSubclassOf<class AAI_Character> TeamClasses;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer")
		uint8 TeamNumber = 0;
};
