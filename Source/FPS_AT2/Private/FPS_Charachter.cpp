// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Charachter.h"
#include "HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"
// Sets default values
AFPS_Charachter::AFPS_Charachter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(50.f, 96.0f);
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	Mesh3P->SetOwnerNoSee(true);


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

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->SetAutoActivate(true);

	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();
	MovementComponent->MaxWalkSpeedCrouched = 200;
	DropWeaponMaxDistance = 200.0f;

	SprintingSpeedModifier = 3.0f;


}

// Called when the game starts or when spawned
void AFPS_Charachter::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AFPS_Charachter::OnHealthChanged);
	if(DefaultWeaponClasses.Num() >= 0)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[0]);
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->AttachMeshToPawn();
		//CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponAttachPoint);
		
	}

	auto it = Cast<AFPS_Charachter>(GetOwner());
	if (it)
	{	
		USkeletalMeshComponent* PawnMesh1p = it->GetMeshComp(false);
		//CurrentWeapon->attacht
		//Mesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, it->WeaponAttachPoint);
	}
}


void AFPS_Charachter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPS_Charachter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPS_Charachter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPS_Charachter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPS_Charachter::EquipWeapon(AWeaponBase* Weapon)
{
	if(Weapon && Weapon != CurrentWeapon)
	{
		SetCurrentWeapon(CurrentWeapon, Weapon);
	}
}

void AFPS_Charachter::SetCurrentWeapon(AWeaponBase* PreviousWeapon, class AWeaponBase* NewWeapon)
{
	if (PreviousWeapon == nullptr || PreviousWeapon == NewWeapon)
	{
		CurrentWeapon = NewWeapon;
	}
	if (PreviousWeapon)
	{
		PreviousWeapon->Unequip();
	}
	NewWeapon->Equip();
}

void AFPS_Charachter::DropWeapon(AWeaponBase* Weapon)
{
	//@TODO Drop weapon upon pressing F
	if(Inventory.Num() < 1)
		return;
	if(!Weapon)
		return;
	//if (Inventory.Contains(Weapon))
	//{
	//	Weapon->OnLeaveInventory();
	//	Inventory.RemoveSingle(Weapon);
	//}
	if (CurrentWeapon)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		FVector SpawnLocation;
		FRotator SpawnRotation = CameraRotation;
		//line trace on where to drop
		const FVector LinetraceStart = GetActorLocation();
		const FVector LinetraceEnd = LinetraceStart + (CameraRotation.Vector() * DropWeaponMaxDistance);
		
		FHitResult Hit;
		FCollisionQueryParams CollisionParams;
		GetWorld()->LineTraceSingleByChannel(Hit, LinetraceStart, LinetraceEnd, ECC_WorldDynamic, CollisionParams);
		if(Hit.bBlockingHit)
			SpawnLocation = Hit.ImpactPoint + (Hit.ImpactNormal * 20);
		else 
			SpawnLocation = LinetraceEnd;
		FActorSpawnParameters SpawnParams;
		AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(CurrentWeapon->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
	
	SetCurrentWeapon(CurrentWeapon, Inventory[0]);

	Weapon->Destroy();
	
}


void AFPS_Charachter::OnDropWeapon()
{
	DropWeapon(CurrentWeapon);
	UE_LOG(LogTemp, Warning, TEXT("DROP WEAPON"));
}

void AFPS_Charachter::RemoveWeapon(AWeaponBase* Weapon)
{
	if(!(Weapon))
		return;
	Weapon->Unequip();
	Inventory.Remove(Weapon);
}

void AFPS_Charachter::AddWeapon(AWeaponBase* Weapon)
{
	if (!(Weapon))
		return;
	Weapon->SetOwningPawn(this);
	Inventory.AddUnique(Weapon);
}

void AFPS_Charachter::Reload()
{
	//isReloading = true;
	CurrentWeapon->Reload();
	//isReloading = false;
}

void AFPS_Charachter::StartFire()
{
	if (bAllowedToFire && CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AFPS_Charachter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AFPS_Charachter::NextWeapon()
{
	if (Inventory.Num() > 1)
	{
		int32 Index = (Inventory.Find(CurrentWeapon, Index) + 1) % Inventory.Num();
		EquipWeapon(Inventory[Index]);
	}
}

void AFPS_Charachter::PreviousWeapon()
{
	if (Inventory.Num() > 1)
	{
		int32 Index = (Inventory.Find(CurrentWeapon, Index) - 1 + Inventory.Num()) % Inventory.Num();
		EquipWeapon(Inventory[Index]);
	}
}

bool AFPS_Charachter::CanFire()
{
	if(!isReloading)
		return true;
	return false;
}


void AFPS_Charachter::BeginCrouch()
{
	Crouch();
}

void AFPS_Charachter::EndCrouch()
{
	UnCrouch();
}

void AFPS_Charachter::Jump()
{
	Super::Jump();
	bIsJumpingToggled = true;
}

void AFPS_Charachter::StopJumping()
{
	Super::StopJumping();
	bIsJumpingToggled = false;
}

void AFPS_Charachter::Suicide()
{
	UGameplayStatics::ApplyDamage(this, 1000, GetController()->GetInstigatorController(), this, UDamageType::StaticClass());
}

void AFPS_Charachter::StartSprinting()
{
	if (GetVelocity().IsZero() || FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) < .1f)
		MovementComponent->MaxWalkSpeed = 200;
	MovementComponent->MaxWalkSpeed = 600;
	bShouldSprint = true;
	StopFire();
	bAllowedToFire = false;

}

void AFPS_Charachter::StopSprinting()
{
	bShouldSprint = false;
	bAllowedToFire = true;
}

// Called to bind functionality to input
void AFPS_Charachter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPS_Charachter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPS_Charachter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPS_Charachter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPS_Charachter::EndCrouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPS_Charachter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPS_Charachter::StopFire);
	PlayerInputComponent->BindAction("Suicide", IE_Pressed, this, &AFPS_Charachter::Suicide);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFPS_Charachter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFPS_Charachter::StopSprinting);
	PlayerInputComponent->BindAction("Drop_Item", IE_Pressed, this, &AFPS_Charachter::OnDropWeapon);
	
	//@TODO: NEXT WEAPON AND PREVIOUS WEAPON SETUP SUING MOUSE WHEEL
	//@TODO: DROP WITH G
	//@TODO: EQUIP WITH E IF LINE TRACE

	// Enable touchscreen input
	//EnableTouchscreenMovement(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFPS_AT2Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPS_Charachter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPS_Charachter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPS_Charachter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPS_Charachter::LookUpAtRate);

}

void AFPS_Charachter::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bInactive)
	{
		bInactive = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AController* CurrentController = GetController();
		if (CurrentController)
		{
			CurrentController->UnPossess();
			//CurrentController->StopMovement();
			CurrentController->Destroy();
		}

		DetachFromControllerPendingDestroy();
		UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
		
	}
}


//void AFPS_Charachter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	//DOREPLIFETIME(AFPS_Charachter, Spawned_weapon);
//	//DOREPLIFETIME(AFPS_Charachter, bInactive);
//}


void AFPS_Charachter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bShouldSprint)
	{
		UnCrouch();
	}

	//@TODO: pick up based on what's seen throiugh linetrace from eye to dropped equipment


}