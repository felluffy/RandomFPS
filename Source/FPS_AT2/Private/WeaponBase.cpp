// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Bullet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "FPS_Charachter.h"
#include "GameFramework/InputSettings.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DisplayDebugHelpers.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	RootComponent = FP_Gun;
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetCollisionObjectType(ECC_WorldDynamic);
	FP_Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FP_Gun->SetCollisionResponseToAllChannels(ECR_Ignore);
	FP_Gun->SetHiddenInGame(true);



	TP_Gun->SetOnlyOwnerSee(false);
	TP_Gun->bCastDynamicShadow = true;
	TP_Gun->CastShadow = true;
	TP_Gun->SetCollisionObjectType(ECC_WorldDynamic);
	TP_Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TP_Gun->SetCollisionResponseToAllChannels(ECR_Ignore);
	TP_Gun->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//SetReplicates(true);
	TP_Gun->SetupAttachment(FP_Gun);
	TP_Gun->SetHiddenInGame(true);
	//UE_LOG(LogTemp, Error, TEXT("Weapon cons called %s - %d - %d"), *GetName(), CurrentAmmo, CurrentAmmoInMagazine);

	//Networking here
}

void AWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &AWeaponBase::DropOnWorld, .1, false);
}


void AWeaponBase::SetOwningPawn(AFPS_Charachter* NewOwner)
{
	if (Owner != NewOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s -"), *NewOwner->GetName());
		bAllowedToFire = true;
		SetInstigator(NewOwner);
		Owner = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);	
		FP_Gun->SetOnlyOwnerSee(true);
		FP_Gun->bCastDynamicShadow = false;
		FP_Gun->CastShadow = false;
		FP_Gun->SetCollisionObjectType(ECC_WorldDynamic);
		FP_Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FP_Gun->SetCollisionResponseToAllChannels(ECR_Ignore);
		FP_Gun->SetHiddenInGame(true);
		this->SetActorTickEnabled(true);

	}
}

void AWeaponBase::DropOnWorld()
{
	if (!Owner)
	{
		FP_Gun->SetCollisionObjectType(ECC_WorldStatic);
		FP_Gun->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FP_Gun->AddLocalRotation({ 3, 6, 6 });
		FP_Gun->CastShadow = true;
		//FP_Gun->SetCollisionResponseToAllChannels(ECR_Ignore);
		FP_Gun->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		FP_Gun->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		FP_Gun->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		FP_Gun->SetHiddenInGame(false);
		FP_Gun->SetSimulatePhysics(true);
		FP_Gun->SetOnlyOwnerSee(false);
		this->SetActorTickEnabled(false);
		//DrawDebugSphere(GetWorld(), FP_Gun->GetComponentLocation(), 50, 8, FColor::Red, true, 10);
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	
	if (bRecoil)
	{
		Owner->AddControllerPitchInput(CurrentRecPitch * DeltaTime * RecoilTimeModifier);
		Owner->AddControllerYawInput(CurrentRecYaw* DeltaTime * RecoilTimeModifier);
	}
}

void AWeaponBase::DroppedOnWorld()
{
	FP_Gun->SetSimulatePhysics(true);
	FP_Gun->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	FP_Gun->AddTorqueInRadians(FVector(1, 1, 1) * 4000000);

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoInMagazine = MagazineAmmoCapacity;
	CurrentAmmo = MaxAmmo - CurrentAmmoInMagazine;
	FMath::Clamp<int>(CurrentAmmo, 0, MaxAmmo);
	if (GetOwner() != NULL)
	{
		auto name = *GetOwner()->GetName();
		//UE_LOG(LogTemp, Warning, TEXT("%s is the owner"), *name);
		
	}
	//UE_LOG(LogTemp, Warning, TEXT("%d - %d"), CurrentAmmoInMagazine, CurrentAmmo);

}

void AWeaponBase::OnFire()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *ProjectileClass->GetName());
	if(bAllowedToFire && Owner)
	if (ProjectileClass != NULL)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%d - %d"), CurrentAmmoInMagazine, CurrentAmmo);
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			bRecoil = false;
			if (CurrentAmmoInMagazine) // can fire
			{
				//UE_LOG(LogTemp, Warning, TEXT("FIRING"));
				//const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
				bRecoil = false;
				FRotator SpawnRotation;

				auto Velocity = Owner->GetVelocity();
				auto Clamped = ClampVector(Velocity, FVector(0.0), FVector(1.0));
				auto VectorSize = Clamped.Size();
				
				FVector SpreadAdjustment = (FVector(FMath::RandRange(-Spread, Spread) * VectorSize, FMath::RandRange(-Spread, Spread) * VectorSize, 0.0));
		
				//SpawnRotation = Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetComponentRotation() + SpreadAdjustment.ToOrientationRotator();;
				SpawnRotation = Owner->GetControlRotation() + SpreadAdjustment.ToOrientationRotator();;
				//auto c = 
				auto Rot = Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetComponentRotation();
				auto Rot2 = SpreadAdjustment.ToOrientationRotator();
				//UE_LOG(LogTemp, Warning, TEXT(" CYKA BLYAT ROT_SPAWNRO %f %f %f"), Rot2.Yaw, Rot2.Pitch, Rot2.Roll);
				//UE_LOG(LogTemp, Warning, TEXT(" CYKA BLYAT ROT_CAMERA %f %f %f"), Rot.Yaw, Rot.Pitch, Rot.Roll);
				FVector SpawnLocation;
				if(IsFirstPerson)
				{
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
				}

				//else 
					//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				ActorSpawnParams.Instigator = Owner;
				ActorSpawnParams.Owner = Owner;
				// spawn the projectile at the muzzle
				auto bbb = World->SpawnActor<ABullet>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				bbb->fDamage = this->WeaponDamage;
				bbb->bIsExplosive = this->IsExplosive;
				bbb->SetInstigator(Owner);
				CurrentAmmoInMagazine--;
				if (CurrentAmmoInMagazine <= 0)
					bAllowedToFire = false;

				//Add Recoil

				OnRecoil();

				//if (ensure(FireSound))
				//{
				//	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, SpawnLocation);
				//}

				//if (ensure(MuzzleEffect))
				//{
				//	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, FP_Gun, MuzzleSocketName);
				//}
			}
			
		}
	}
	else
	{
		Reload();
	}
}


void AWeaponBase::OnRecoil()
{
	//Owner->AddControllerPitchInput(FMath::RandRange(RandomRecoilPitchLow, RandomRecoilPitchHigh));
	//Owner->AddControllerYawInput(FMath::RandRange(RandomRecoilYawLow, RandomRecoilYawHigh));
	CurrentRecPitch = FMath::RandRange(RandomRecoilPitchLow, RandomRecoilPitchHigh);
	CurrentRecYaw = FMath::RandRange(RandomRecoilYawLow, RandomRecoilYawHigh);
	bRecoil = true;
		
}

bool AWeaponBase::CanFire()
{
	if (CurrentAmmoInMagazine)
		return true;
	return false;
}

void AWeaponBase::WeaponFire()
{
	OnFire();
}

void AWeaponBase::StartFire()
{
	float FirstDelay = FMath::Max(fLastFireTime + fIntervalShootingTime - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_fIntervalShootingTime, this, &AWeaponBase::OnFire, fIntervalShootingTime, true, FirstDelay);
}


void AWeaponBase::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_fIntervalShootingTime);
	bRecoil = false;
}


void AWeaponBase::AttachMeshToPawn()
{
	if (Owner)
	{
		USkeletalMeshComponent* OwnerMesh1P = Owner->GetMeshComp(true);
		USkeletalMeshComponent* OwnerMesh3P = Owner->GetMeshComp(false);

		FName AttachPoint = Owner->WeaponAttachPoint;

		FP_Gun->SetHiddenInGame(false);
		TP_Gun->SetHiddenInGame(false);
		FP_Gun->SetOnlyOwnerSee(true);
		TP_Gun->SetOwnerNoSee(true);
		
		
		FP_Gun->AttachToComponent(OwnerMesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), AttachPoint);
		TP_Gun->AttachToComponent(OwnerMesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), AttachPoint);
	}
}

void AWeaponBase::Unequip()
{
	this->SetActorTickEnabled(false);
	FP_Gun->SetHiddenInGame(true);
	TP_Gun->SetHiddenInGame(true);
	bAllowedToFire = false;
	StopFire();
}

void AWeaponBase::Equip()
{
	this->SetActorTickEnabled(true);
	FP_Gun->SetHiddenInGame( false );
	TP_Gun->SetHiddenInGame(false);
	bAllowedToFire = true;
	AttachMeshToPawn();
}

void AWeaponBase::Reload()
{
	StopFire();
	
	//reload conditions
	//UE_LOG(LogTemp, Warning, TEXT("RELOADING"));
	if (CurrentAmmoInMagazine == MagazineAmmoCapacity || CurrentAmmo <= 0)
		return;
	float FirstDelay = FMath::Max(fLastFireTime + fIntervalShootingTime - GetWorld()->TimeSeconds, 0.0f);
	
	//GetWorldTimerManager().SetTimer(TimerHandle_fReloadTime, 5.0f, false);
	GetWorldTimerManager().SetTimer(TimerHandle_fReloadTime, this, &AWeaponBase::ReloadWeapon, ReloadTime, false);

	//Play Reload Animation

	

}

void AWeaponBase::ReloadWeapon()
{
	CurrentAmmo += CurrentAmmoInMagazine;
	if (CurrentAmmo >= MagazineAmmoCapacity)
		CurrentAmmoInMagazine = MagazineAmmoCapacity;
	else
		CurrentAmmoInMagazine = CurrentAmmo % (MagazineAmmoCapacity + 1);
	CurrentAmmo -= CurrentAmmoInMagazine;
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, GetActorLocation());
	}
	bAllowedToFire = true;
}

void AWeaponBase::SetCurrentAmmo(int32 _CurrentAmmo, int32 _CurrentAmmoInMagazine)
{
	this->CurrentAmmo = _CurrentAmmo;
	this->CurrentAmmoInMagazine = _CurrentAmmoInMagazine;
}
