// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FPS_AT2Character.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Net/UnrealNetwork.h"
#include "WeaponBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HealthComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPS_AT2Character

AFPS_AT2Character::AFPS_AT2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(50.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;


	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Create a gun mesh component
	//FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	//FP_Gun->bCastDynamicShadow = false;
	//FP_Gun->CastShadow = false;
	//// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	//FP_Gun->SetupAttachment(RootComponent);

	//FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	//FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	//// Default offset from the character location for projectiles to spawn
	//GunOffset = FVector(100.0f, 0.0f, 10.0f);

	//// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	//// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	//// Create VR Controllers.
	//R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	//R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	//R_MotionController->SetupAttachment(RootComponent);
	//L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	//L_MotionController->SetupAttachment(RootComponent);

	//// Create a gun and attach it to the right-hand VR controller.
	//// Create a gun mesh component
	//VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	//VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	//VR_Gun->bCastDynamicShadow = false;
	//VR_Gun->CastShadow = false;
	//VR_Gun->SetupAttachment(R_MotionController);
	//VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	//VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	//VR_MuzzleLocation->SetupAttachment(VR_Gun);
	//VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	//VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AFPS_AT2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	Spawned_weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponBP);
	Spawned_weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	//Spawned_weapon->AnimInstance = Mesh1P->GetAnimInstance();

	HealthComp->OnHealthChanged.AddDynamic(this, &AFPS_AT2Character::OnHealthChanged);	

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	//FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	//// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	//if (bUsingMotionControllers)
	//{
	//	VR_Gun->SetHiddenInGame(false, true);
	//	Mesh1P->SetHiddenInGame(true, true);
	//}
	//else
	//{
	//	VR_Gun->SetHiddenInGame(true, true);
	//	Mesh1P->SetHiddenInGame(false, true);
	//}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPS_AT2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPS_AT2Character::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPS_AT2Character::EndCrouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPS_AT2Character::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPS_AT2Character::StopFire);
	

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFPS_AT2Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPS_AT2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPS_AT2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPS_AT2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPS_AT2Character::LookUpAtRate);
}

//void AFPS_AT2Character::OnFire()
//{
//	// try and fire a projectile
//	if (ProjectileClass != NULL)
//	{
//		UWorld* const World = GetWorld();
//		if (World != NULL)
//		{
//			if (bUsingMotionControllers)
//			{
//				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
//				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
//				World->SpawnActor<AFPS_AT2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
//			}
//			else
//			{
//				const FRotator SpawnRotation = GetControlRotation();
//				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
//				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
//
//				//Set Spawn Collision Handling Override
//				FActorSpawnParameters ActorSpawnParams;
//				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
//				// spawn the projectile at the muzzle
//				World->SpawnActor<AFPS_AT2Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
//			}
//		}
//	}
//
//	// try and play the sound if specified
//	if (FireSound != NULL)
//	{
//		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
//	}
//
//	// try and play a firing animation if specified
//	if (FireAnimation != NULL)
//	{
//		// Get the animation object for the arms mesh
//		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
//		if (AnimInstance != NULL)
//		{
//			AnimInstance->Montage_Play(FireAnimation, 1.f);
//		}
//	}
//}

//void AFPS_AT2Character::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

void AFPS_AT2Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		StartFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPS_AT2Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		StopFire();
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AFPS_AT2Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AFPS_AT2Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPS_AT2Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPS_AT2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPS_AT2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AFPS_AT2Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_AT2Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPS_AT2Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFPS_AT2Character::TouchUpdate);
		return true;
	}
	
	return false;
}


void AFPS_AT2Character::StartFire()
{

	if (ensure(Spawned_weapon != nullptr))
		Spawned_weapon->StartFire();
	//spawned_weapon->SetFiring(true);
	//UE_LOG(LogTemp, Warning, TEXT("Start fire fps char"));
}

void AFPS_AT2Character::StopFire()
{

	if (ensure(Spawned_weapon != nullptr))
		Spawned_weapon->StopFire();
	//spawned_weapon->SetFiring(false);
	//UE_LOG(LogTemp, Warning, TEXT("Stop fire fps char"));
}

//
void AFPS_AT2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPS_AT2Character, Spawned_weapon);
	DOREPLIFETIME(AFPS_AT2Character, bInactive);
}

void AFPS_AT2Character::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("CYKA BLYAT CHECK HERE"));
	if (Health <= 0.0f && !bInactive)
	{
		bInactive = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AController* CurrentController = GetController();
		if (CurrentController)
		{
			CurrentController->UnPossess();
			CurrentController->StopMovement();
			CurrentController->Destroy();
		}

		DetachFromControllerPendingDestroy();
		UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
		Destroy();
	}
}


void AFPS_AT2Character::BeginCrouch()
{
	Crouch();
}

void AFPS_AT2Character::EndCrouch()
{
	UnCrouch();
}