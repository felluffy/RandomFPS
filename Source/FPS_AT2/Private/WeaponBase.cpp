// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Bullet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include <ctime>
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "FPS_Charachter.h"
#include "AI_Character.h"
#include "GameFramework/InputSettings.h"
#include "FPS_Charachter.h"
#include "FPS_AT2PlayerController.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	bAddedOffset = false;
	RS.Initialize(time(0));//= srand(time(0));
	//ZoomCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ZoomCameraComponent"));
	//ZoomCameraComponent->SetupAttachment(GetRootComponent());

	//ZoomCameraComponent->SetRelativeScale3D({ .05,.05,.05 });
	//ZoomCameraComponent->SetRelativeLocation(VectorToTransform.InverseTransformLocation((FP_Gun->GetSocketTransform(ZoomSocketName))))


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

		auto IsAI = Cast<AAI_Character>(NewOwner);
		if (!IsAI)
		{
			bShouldAddRecoilAI = true;
		}

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
		bShouldAddRecoilAI = false;
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	//DrawDebugSphere(GetWorld(), FP_Gun->GetSocketTransform(ZoomSocketName, RTS_Component).GetLocation(), 5, 2, FColor::Red, 10, .2, 0, 2);
	if (bRecoil && bShouldAddRecoilAI)
	{
		Owner->AddControllerPitchInput(CurrentRecPitch * DeltaTime * RecoilTimeModifier);
		Owner->AddControllerYawInput(CurrentRecYaw* DeltaTime * RecoilTimeModifier);
	}
}

void AWeaponBase::DroppedOnWorld()
{
	FP_Gun->SetSimulatePhysics(true);
	FP_Gun->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	FP_Gun->AddTorqueInRadians(FVector(1, .5, .5) * 40000);

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	//ZoomCameraComponent->SetRelativeLocation(FP_Gun->GetSocketTransform(ZoomSocketName, ERelativeTransformSpace::RTS_Component).GetLocation());
	//ZoomCameraComponent->SetWorldRotation(FP_Gun->GetSocketRotation(ZoomSocketName));
	//ZoomCameraComponent->Deactivate();
	//DrawDebugSphere(GetWorld(), ZoomCameraComponent->GetComponentLocation(), 15, 3, FColor::Red, false, 15, 0, 2);
	//UE_LOG(LogTemp, Warning, TEXT("C++ %s begin play called"), *GetName());
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
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ProjectileClass->GetName());
	if (!CurrentAmmoInMagazine)
	{
		StopFire();
		if (EmptyClip && EmptyClipAttenuation)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyClip, GetActorLocation(), 1, 1, 0, EmptyClipAttenuation);
		}
		Reload();
		return;
	}

	if (bAllowedToFire && Owner)
	{
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
					
					auto VectorSize = Velocity.Size();
					VectorSize = (VectorSize / 1000.0f) + .05f;
					//UE_LOG(LogTemp, Warning, TEXT("VECTOR SIZE SPEED : %f"), VectorSize);
					auto AI_Bot = Cast<AAI_Character>(GetOwner());

					FVector SpreadAdjustment = FVector::ZeroVector;
					if(FP_MuzzleLocation)
					{
						//SpreadAdjustment = RS.VRandCone(FP_MuzzleLocation->Getworldloc, Spread, Spread);//(FVector(FMath::RandRange(-Spread, Spread) * VectorSize, FMath::RandRange(-Spread, Spread) * VectorSize, 0.0));
						if(!bZoomed)
							SpreadAdjustment = RS.VRandCone(Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetForwardVector(), Spread * VectorSize, Spread * VectorSize);
						else
							SpreadAdjustment = RS.VRandCone(Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetForwardVector(), Spread * VectorSize / 10, Spread * VectorSize / 10);
					
					}
					SpawnRotation = UKismetMathLibrary::MakeRotFromX(SpreadAdjustment);
					//SpawnRotation = SpreadAdjustment.ToOrientationRotator();
					/*if (AI_Bot)
						SpreadAdjustment += {.1, .1, .1};*/
					//SpawnRotation = Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetComponentRotation() + SpreadAdjustment.ToOrientationRotator();;
					//SpawnRotation = Owner->GetControlRotation() + SpreadAdjustment.ToOrientationRotator();;
					//SpawnRotation = UKismetMathLibrary::ComposeRotators(Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetComponentRotation(), SpreadAdjustment.ToOrientationRotator());
						
				/*	auto Rot = Cast<AFPS_Charachter>(GetOwner())->GetFirstPersonCameraComponent()->GetComponentRotation();
					auto Rot2 = SpreadAdjustment.ToOrientationRotator();*/
					////UE_LOG(LogTemp, Warning, TEXT(" CYKA BLYAT ROT_SPAWNRO %f %f %f"), Rot2.Yaw, Rot2.Pitch, Rot2.Roll);
					////UE_LOG(LogTemp, Warning, TEXT(" CYKA BLYAT ROT_CAMERA %f %f %f"), Rot.Yaw, Rot.Pitch, Rot.Roll);
					FVector SpawnLocation;
					//if (IsFirstPerson)
					{
						FQuat Rot;
						// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
						SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation());// + SpawnRotation.RotateVector(GunOffset); TP_Gun->GetSocketWorldLocationAndRotation(MuzzleSocketName, SpawnLocation
						/*SpawnLocation = ((AI_Bot != nullptr) ? , Rot).)*/
					}
					//DrawDebugSphere(GetWorld(), FP_MuzzleLocation->GetComponentLocation(), 10, 2, FColor::Yellow, false, 3, 0, 5);
					//else 
					//	SpawnLocation = ((TP_Gun->GetSocketByName(MuzzleSocketName) != NULL) ? TP_Gun->GetSocketWorldLocationAndRotation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
					ActorSpawnParams.Instigator = Owner;
					ActorSpawnParams.Owner = Owner;
					// spawn the projectile at the muzzle
					//FTransform Transform = {()}
					//World->spawnactor
					auto bbb = World->SpawnActor<ABullet>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
					if (bbb)
					{
						bbb->fDamage = this->WeaponDamage;
						bbb->bIsExplosive = this->IsExplosive;
						//bbb->SetInstigator(Owner);
					}
					//UGameplayStatics::FinishSpawningActor(bbb);
					CurrentAmmoInMagazine--;
					if (CurrentAmmoInMagazine <= 0)
						bAllowedToFire = false;

					//Add Recoil

					OnRecoil();

					if (FireSound != NULL)
					{
						MakeNoise(FireSoundScale, Owner, SpawnLocation, FireSoundRange);
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, SpawnLocation, 1, 1, 0, FireSoundAttenuation );
					}

					if (MuzzleEffect != NULL)
					{
						UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, FP_Gun, MuzzleSocketName);
					}

					if (FireShake != NULL)
					{
						if (Owner->GetPlayerAFPSController() != NULL)
							Owner->GetPlayerAFPSController()->PlayerCameraManager->PlayCameraShake(FireShake, FireShakeAlpha, ECameraAnimPlaySpace::	CameraLocal);
					}
					//if (FireAnimation != NULL)
					//{
					//	//Owner->PlayFireAnimation();
					//}
					if(!bZoomed)
						PlayWeaponAnimation(FireAnimation);
					else 
						PlayWeaponAnimation(FireAnimationSights);

				}
			}
		}
	}
	else
	{
		Reload();
	}
}


float AWeaponBase::PlayWeaponAnimation(const FWeaponAnim& Animation, float InPlayRate)
{
	float Duration = 0.0f;
	if (Owner)
	{
		
		if (Animation.Pawn3P)
		{
			Duration = Owner->PlayAnimMontageOnMesh(Animation.Pawn3P, InPlayRate, NAME_None, false);
		}
		UAnimMontage* AnimToPlay = Owner->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (AnimToPlay)
		{
			Duration = Owner->PlayAnimMontageOnMesh(AnimToPlay, InPlayRate, NAME_None, true);
		}
	}

	return Duration;
}

void AWeaponBase::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	if (Owner)
	{
		Owner->StopAllMontage();
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

//bool AWeaponBase::ShouldLagRot()
//{
//	return bProceduralLagRotation;
//}
//
//float AWeaponBase::LagWeaponRotation(float NewValue, float LastValue, float DeltaTime, float MaxDiff, int32 Index)
//{
//	NewValue = FMath::UnwindDegrees(NewValue);
//	LastValue = FMath::UnwindDegrees(LastValue);
//
//	float LeadMag = 0.f;
//	float RotDiff = NewValue - LastValue;
//	if ((RotDiff == 0.f) || (OldRotDiff[Index] == 0.f))
//	{
//		LeadMag = ShouldLagRot() ? OldLeadMag[Index] : 0.f;
//		if ((RotDiff == 0.f) && (OldRotDiff[Index] == 0.f))
//		{
//			OldMaxDiff[Index] = 0.f;
//		}
//	}
//	else if ((RotDiff > 0.f) == (OldRotDiff[Index] > 0.f))
//	{
//		if (ShouldLagRot())
//		{
//			MaxDiff = FMath::Min(1.f, FMath::Abs(RotDiff) / (66.f * DeltaTime)) * MaxDiff;
//			if (OldMaxDiff[Index] != 0.f)
//			{
//				MaxDiff = FMath::Max(OldMaxDiff[Index], MaxDiff);
//			}
//
//			OldMaxDiff[Index] = MaxDiff;
//			LeadMag = (NewValue > LastValue) ? -1.f * MaxDiff : MaxDiff;
//		}
//		LeadMag = (DeltaTime < 1.f / RotChgSpeed)
//			? LeadMag = (1.f - RotChgSpeed * DeltaTime)*OldLeadMag[Index] + RotChgSpeed * DeltaTime*LeadMag
//			: LeadMag = 0.f;
//	}
//	else
//	{
//		OldMaxDiff[Index] = 0.f;
//		if (DeltaTime < 1.f / ReturnChgSpeed)
//		{
//			LeadMag = (1.f - ReturnChgSpeed * DeltaTime)*OldLeadMag[Index] + ReturnChgSpeed * DeltaTime*LeadMag;
//		}
//	}
//	OldLeadMag[Index] = LeadMag;
//	OldRotDiff[Index] = RotDiff;
//
//	return LeadMag;
//}
//
//void AWeaponBase::UpdateWeaponHand()
//{
//	if (Mesh != NULL && UTOwner != NULL)
//	{
//		FirstPMeshOffset = FVector::ZeroVector;
//		FirstPMeshRotation = FRotator::ZeroRotator;
//
//		if (MuzzleFlashDefaultTransforms.Num() == 0)
//		{
//			for (UParticleSystemComponent* PSC : MuzzleFlash)
//			{
//				MuzzleFlashDefaultTransforms.Add((PSC == NULL) ? FTransform::Identity : PSC->GetRelativeTransform());
//				MuzzleFlashSocketNames.Add((PSC == NULL) ? NAME_None : PSC->GetAttachSocketName());
//			}
//		}
//		else
//		{
//			for (int32 i = 0; i < FMath::Min3<int32>(MuzzleFlash.Num(), MuzzleFlashDefaultTransforms.Num(), MuzzleFlashSocketNames.Num()); i++)
//			{
//				if (MuzzleFlash[i] != NULL)
//				{
//					MuzzleFlash[i]->AttachToComponent(MuzzleFlash[i]->GetAttachParent(), FAttachmentTransformRules::SnapToTargetIncludingScale, MuzzleFlashSocketNames[i]);
//					MuzzleFlash[i]->bUseAttachParentBound = true;
//					MuzzleFlash[i]->SetRelativeTransform(MuzzleFlashDefaultTransforms[i]);
//				}
//			}
//		}
//
//		// If we're attached, make sure that we update to the right hands socket
//		if (Mesh->GetAttachParent() && Mesh->GetAttachSocketName() != HandsAttachSocket)
//		{
//			Mesh->AttachToComponent(Mesh->GetAttachParent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false), HandsAttachSocket);
//		}
//
//		if (HandsAttachSocket == NAME_None)
//		{
//			UTOwner->FirstPersonMesh->SetRelativeTransform(FTransform::Identity);
//		}
//		else
//		{
//			USkeletalMeshComponent* DefaultHands = UTOwner->GetClass()->GetDefaultObject<AUTCharacter>()->FirstPersonMesh;
//			UTOwner->FirstPersonMesh->RelativeLocation = DefaultHands->RelativeLocation;
//			UTOwner->FirstPersonMesh->RelativeRotation = DefaultHands->RelativeRotation;
//			UTOwner->FirstPersonMesh->RelativeScale3D = DefaultHands->RelativeScale3D;
//			UTOwner->FirstPersonMesh->UpdateComponentToWorld();
//		}
//
//		USkeletalMeshComponent* AdjustMesh = (HandsAttachSocket != NAME_None) ? UTOwner->FirstPersonMesh : Mesh;
//		USkeletalMeshComponent* AdjustMeshArchetype = Cast<USkeletalMeshComponent>(AdjustMesh->GetArchetype());
//
//		switch (GetWeaponHand())
//		{
//		case EWeaponHand::HAND_Center:
//			// TODO: not implemented, fallthrough
//			UE_LOG(UT, Warning, TEXT("HAND_Center is not implemented yet!"));
//		case EWeaponHand::HAND_Right:
//			AdjustMesh->SetRelativeLocationAndRotation(AdjustMeshArchetype->RelativeLocation, AdjustMeshArchetype->RelativeRotation);
//			break;
//		case EWeaponHand::HAND_Left:
//		{
//			// TODO: should probably mirror, but mirroring breaks sockets at the moment (engine bug)
//			AdjustMesh->SetRelativeLocation(AdjustMeshArchetype->RelativeLocation * FVector(1.0f, -1.0f, 1.0f));
//			FRotator AdjustedRotation = (FRotationMatrix(AdjustMeshArchetype->RelativeRotation) * FScaleMatrix(FVector(1.0f, 1.0f, -1.0f))).Rotator();
//			AdjustMesh->SetRelativeRotation(AdjustedRotation);
//			break;
//		}
//		case EWeaponHand::HAND_Hidden:
//		{
//			AdjustMesh->SetRelativeLocationAndRotation(FVector(-50.0f, 0.0f, -50.0f), FRotator::ZeroRotator);
//			if (AdjustMesh != Mesh)
//			{
//				Mesh->AttachToComponent(Mesh->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
//				Mesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
//			}
//			for (int32 i = 0; i < MuzzleFlash.Num() && i < MuzzleFlashDefaultTransforms.Num(); i++)
//			{
//				if (MuzzleFlash[i] != NULL)
//				{
//					MuzzleFlash[i]->AttachToComponent(MuzzleFlash[i]->GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
//					MuzzleFlash[i]->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
//					MuzzleFlash[i]->bUseAttachParentBound = false;
//				}
//			}
//			break;
//		}
//		}
//	}
//}

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
		//ZoomCameraComponent->AttachToComponent(FP_Gun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), ZoomSocketName);
		//FP_Gun->SetWorldLocation({ Owner->GetActorForwardVector() * 30 });
		auto RootBoneLocation_FP = FP_Gun->GetBoneLocation(RootBoneName);
		auto HandleLocation_FP = FP_Gun->GetSocketLocation(GripHandle);

		auto RootBoneLocation_TP = TP_Gun->GetBoneLocation(RootBoneName);
		auto HandleLocation_TP = TP_Gun->GetSocketLocation(GripHandle);

		auto Difference_FP = RootBoneLocation_FP - HandleLocation_FP;//FVector::Distance(RootBoneLocation, HandleLocation);
		auto Difference_TP = RootBoneLocation_TP - HandleLocation_TP;
		if (!bAddedOffset)
		{
			FP_Gun->AddWorldOffset(Difference_FP);
			TP_Gun->AddWorldOffset(Difference_TP);

			bAddedOffset = true;
		}
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
	FP_Gun->SetHiddenInGame(false);
	TP_Gun->SetHiddenInGame(false);
	bAllowedToFire = true;
	AttachMeshToPawn();
}

void AWeaponBase::Reload()
{
	if (!bIsReloading)
	{

		StopFire();
		Owner->SetCanSwapWeapons(false);
		if (CurrentAmmoInMagazine == MagazineAmmoCapacity)
			return;
		bAllowedToFire = false;
		bIsReloading = true;
		//float FirstDelay = FMath::Max(fLastFireTime + fIntervalShootingTime - GetWorld()->TimeSeconds, 0.0f);

		float ReloadAnimDuration = PlayWeaponAnimation(ReloadAnimation);
		if (ReloadAnimDuration <= 0)
		{
			ReloadAnimDuration = ReloadTime;
		}
		GetWorldTimerManager().SetTimer(TimerHandle_fReloadTime, this, &AWeaponBase::ReloadWeapon, ReloadAnimDuration, false);

		//Play Reload Animation
		
	}


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
		MakeNoise(ReloadSoundScale, Owner, GetActorLocation(), ReloadSoundRange);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, GetActorLocation());
	}
	StopWeaponAnimation(ReloadAnimation);
	bAllowedToFire = true;
	bIsReloading = false;
	Owner->SetCanSwapWeapons(true);
}

void AWeaponBase::SetCurrentAmmo(int32 _CurrentAmmo, int32 _CurrentAmmoInMagazine)
{
	this->CurrentAmmo = _CurrentAmmo;
	this->CurrentAmmoInMagazine = _CurrentAmmoInMagazine;
}
