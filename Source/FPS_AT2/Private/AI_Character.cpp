// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Character.h"
#include "NPC_AI_Controller.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "VoiceHttpSTTComponent.h"
#include "PatrolComponent.h"

AAI_Character::AAI_Character()
{
	//Super();
	AIControllerClass = ANPC_AI_Controller::StaticClass();
	bUseControllerRotationYaw = true;
	PatrolComp = CreateDefaultSubobject<UPatrolComponent>(TEXT("Patrol Component"));
	VoiceComponent->Deactivate();
	//FirstPersonCameraComponent->Deactivate();	

}

void AAI_Character::BeginPlay()
{
	Super::Super::BeginPlay();
	//Super::BeginPlay();	
	HealthComp->OnHealthChanged.AddDynamic(this, &AAI_Character::OnHealthChanged);
	if (DefaultWeaponClasses.Num() > 0)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[0]);
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->AttachMeshToPawn();
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Inventory.Add(CurrentWeapon);
		if (DefaultWeaponClasses.Num() > 1)
		{
			Inventory.Add(GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[1], SpawnInfo));
			Inventory[1]->SetOwningPawn(this);
		}
		//CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponAttachPoint);
	//UE_LOG(LogTemp, Warning, TEXT("%s - is the current weapon - con %s"), *CurrentWeapon->WeaponName.ToString(), *GetController()->GetName());

	}
}


void AAI_Character::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//UE_LOG(LogTemp, Warning, TEXT("OOF"));
	//if (Health <= 0.0f)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
	//}
	if (Health <= 0.0f && !bInactive)
	{
		bInactive = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//AController* CurrentController = GetController();
		//if (CurrentController)
		//{
		//	CurrentController->UnPossess();
		//	//CurrentController->StopMovement();
		//	CurrentController->Destroy();
		//	/*UE_LOG(LogTemp, Warning, TEXT("%s controller AI_CHAR"), *CurrentController->GetName());*/
		//}
		GetCharacterMovement()->DisableMovement();
		//DetachFromControllerPendingDestroy();
		//UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
		
		Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh3P->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_MAX);
		auto Causer = Cast<AFPS_Charachter>(InstigatedBy->GetCharacter());
		if (Causer != NULL)
		{
			OnDeathNotify.Broadcast(this, Causer);
			UE_LOG(LogTemp, Warning, TEXT("Health at DEAD notified"));
		}
		HealthComponent->Deactivate();
		bDropWeaponOnDeath = true;
		DropWeapon(CurrentWeapon);
		DetachFromControllerPendingDestroy();
		//UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));


	}
}

void AAI_Character::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	GetPerceptionLocationAndRotation(Location, Rotation);
}

void AAI_Character::GetPerceptionLocationAndRotation(FVector& Location, FRotator& Rotation) const
{
	Mesh3P->GetSocketWorldLocationAndRotation(EyeSocketName, Location, Rotation);
}

void AAI_Character::RotateHeadOnGuard()
{
	//HeadRotation = Mesh3P->GetBoneTransform()
	//FMath::RInterpTo(1)
}
