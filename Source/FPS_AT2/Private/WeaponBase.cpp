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
	PrimaryActorTick.bCanEverTick = false;
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	RootComponent = FP_Gun;
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = true;
	FP_Gun->CastShadow = true;
	FP_Gun->SetCollisionObjectType(ECC_WorldDynamic);
	FP_Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FP_Gun->SetCollisionResponseToAllChannels(ECR_Ignore);
	TP_Gun->SetOnlyOwnerSee(false);
	TP_Gun->bCastDynamicShadow = true;
	TP_Gun->CastShadow = true;
	TP_Gun->SetCollisionObjectType(ECC_WorldDynamic);
	TP_Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TP_Gun->SetCollisionResponseToAllChannels(ECR_Ignore);
	//SetReplicates(true);
	TP_Gun->SetupAttachment(FP_Gun);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoInMagazine = MagazineAmmoCapacity;
	CurrentAmmo = MaxAmmo - CurrentAmmoInMagazine;
	FMath::Clamp<int>(CurrentAmmo, 0, MaxAmmo);

}

void AWeaponBase::OnFire()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *ProjectileClass->GetName());
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (CurrentAmmoInMagazine) // can fire
			{

				const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ABullet>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				CurrentAmmoInMagazine--;
				CurrentAmmo--;

				if (ensure(MuzzleEffect))
				{
					UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, FP_Gun, MuzzleSocketName);
				}
			}
		}
	}
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
}

void AWeaponBase::SetOwningPawn(AFPS_Charachter* NewOwner)
{
	if (Owner != NewOwner)
	{
		SetInstigator(NewOwner);
		Owner = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

void AWeaponBase::Unequip()
{
	FP_Gun->SetHiddenInGame(true);
}

void AWeaponBase::Equip()
{
	return;
}

void AWeaponBase::Reload()
{

}