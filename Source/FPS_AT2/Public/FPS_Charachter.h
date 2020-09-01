// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "FPS_Charachter.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeathSignature, AFPS_Charachter*, KilledCharacter, AFPS_Charachter*, KilledByCharacter);
DECLARE_EVENT_TwoParams(AFPS_Charachter, FOnDeathEventSignature, AFPS_Charachter*, AFPS_Charachter*);

UCLASS()
class FPS_AT2_API AFPS_Charachter : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ZoomCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UVoiceHttpSTTComponent* VoiceComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAudioCaptureComponent* AudioCaptureComponent;
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "STT")
	class USynthComponent* SynthComponent;*/

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeathNotify;
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
	//void OnKilledCharacter(class AFPS_Charachter* Killed, class AFPS_Charachter* KilledBy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
		void OnKilledCharacter2Func(class AFPS_Charachter* Killed, class AFPS_Charachter* KilledBy);

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

	UFUNCTION(BlueprintCallable, Category = "Player Movement")
	void BeginCrouch();
	UFUNCTION(BlueprintCallable, Category = "Player Movement")
	void EndCrouch();
	void Jump() override;
	void StopJumping() override;
	void Suicide();
	UFUNCTION(BlueprintCallable, Category = "Player Movement")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Player Movement")
	void StopSprinting();
	void CommandBot();
	void CommandBot_1();
	void CommandBot_2();
	void CommandBot_3();
	void CommandBot_4();
	void OnPressedActionButton();
	void ZoomInToWeapon();
	void ZoomOutFromWeapon();


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
	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
	FRotator HeadRotation;


	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
		bool IsSprinting = false;
	UPROPERTY(BlueprintReadOnly, Category = "Weapons")
		bool IsFiring = false;
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
		bool IsProning = false;
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
		bool IsCrouching = false;
	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
		bool IsAimingDown = false;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float DefaultFieldOfView = 90.0f;
public:
	UPROPERTY(EditDefaultsOnly, Category = "Player Settings")
		float DefaultTraceDistance = 1000;
private:
	FVector CurrentLocationEyeDistanceEnd;
	FHitResult CurrentObjectHit;
public:
	FHitResult GetEyeTrace() { return CurrentObjectHit; }

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
	void AddWeaponToInventory(class AWeaponBase* Weapon);
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")	
	float DropWeaponMaxDistance;
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void Reload();
	bool bCanSwapWeapons = true;
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void NextWeapon();
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void PreviousWeapon();
	bool CanFire();

	bool isReloading = false; //should deactivate on a broadcast from weappon 
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementPlaySound();
	bool bDropWeaponOnDeath = false;
	UAnimMontage* ReloadAnimation = NULL; 
public: 
	//UPROPERTY(BlueprintCallable, Category = "Weapons")
	//	AWeaponBase* GetCurrentWeapon() { return CurrentWeapon; }
	void SetCanSwapWeapons(bool Allowed) { bCanSwapWeapons = Allowed; }
	bool GetCanSwapWeapons() { return bCanSwapWeapons; }

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplayer")
		uint8 TeamNumber = 0;
	void OrderAIAttack();
	void OrderFollow();
	void OrderGuard();
	void OrderDismiss();
	void OrderCallAssistance();
	UFUNCTION(BlueprintCallable)
		void OrderAIGoToLocation(FVector Location, bool FromMap = true);
	UFUNCTION(BlueprintPure)
	virtual class AFPS_AT2PlayerController* GetPlayerAFPSController() const; 
private:
	class AFPS_AT2PlayerController* PlayerController_AFPS2;


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
	void StartRecordingAudio();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events")
	void StopRecordingAudio();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Shakes")
		TSubclassOf<class UCameraShake> OnDamagedShake;
	UPROPERTY(EditDefaultsOnly, Category = "Shakes")
		TSubclassOf<class UCameraShake> OnMovmentShake;

public:
	UFUNCTION(BlueprintCallable)
		virtual void OnVoiceRecognized(UVoiceHttpSTTComponent* STTComponent, float AccuracyScore, FString SentenceRetreived);
	FString RecognizedWord;

public:
	float PlayMontage(class UAnimMontage* MontageToPlay, float InPlayRate, FName StartSectionName);
	void StopAllMontage();

public:
	virtual bool CanBeSeenFrom(
		const FVector& ObserverLocation,
		FVector& OutSeenLocation,
		int32& NumberOfLoSChecksPerformed,
		float& OutSightStrength,
		const AActor* IgnoreActor = NULL
	) const override;

public:
	virtual bool IsFirstPerson() const;
protected:
	FName EyeSocketName = "headSocket";

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
public:
	UFUNCTION(BlueprintCallable, Category = Animation)
		float PlayAnimMontageOnMesh(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None, bool UseFP = false);
private:
	class USkeletalMeshComponent* MeshToUse;  

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widgets")
	void RemoveAllWidgetsFromViewPort();

private:
	void SlideCharacter();
	void StopSliding();
	FTimerHandle SlideTimerHandle;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	class UAnimMontage* SlideAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float SlideTime;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSliding;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RunSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float CrouchSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SlideSpeed;
public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
};