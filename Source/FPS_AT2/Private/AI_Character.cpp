// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Character.h"
#include "NPC_AI_Controller.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "VoiceHttpSTTComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PatrolComponent.h"

AAI_Character::AAI_Character()
{
	//Super();
	AIControllerClass = ANPC_AI_Controller::StaticClass();
	bUseControllerRotationYaw = true;
	PatrolComp = CreateDefaultSubobject<UPatrolComponent>(TEXT("Patrol Component"));
	VoiceComponent->Deactivate();
	AreaContextBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaContextBox"));
	//FirstPersonCameraComponent->Deactivate();	

}

void AAI_Character::BeginPlay()
{
	Super::Super::BeginPlay();
	//Super::BeginPlay();	
	HealthComp->OnHealthChanged.AddDynamic(this, &AAI_Character::OnHealthChanged);
	HealthComp->SetTeam(TeamNumber);
	GetWorldTimerManager().SetTimer(SetupTeamsTimer, this, &AFPS_Charachter::SetupTeams, DelaySetup, false);
	if (DefaultWeaponClasses.Num() > 0)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//SpawnInfo.Owner = this;
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[0], SpawnInfo);
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->AttachMeshToPawn();

		//ZoomCameraComponent->SetWorldLocation(CurrentWeapon->GetFP_Gun()->GetSocketLocation("RifleZoomSocket"));
		//ZoomCameraComponent->SetWorldRotation(CurrentWeapon->GetFP_Gun()->GetSocketRotation("RifleZoomSocket"));

		//ZoomCameraComponent->Attach

		Inventory.Add(CurrentWeapon);
		for (int i = 1; i < DefaultWeaponClasses.Num(); i++)
		{
			Inventory.Add(GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[i], SpawnInfo));
			Inventory[i]->SetOwningPawn(this);
		}
	}
	DefaultWeaponClasses.Empty();

	this->OnDeathAddDeathToServer.AddDynamic(this, &AFPS_Charachter::OnDeathAddDeathToServer_Func);
	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPS_Charachter::StaticClass(), FoundActors);
	//for (auto &it : FoundActors)
	//{
	//	auto Character = Cast<AFPS_Charachter>(it);

	//	if (Character && Character != this)
	//		this->OnDeathNotify.AddDynamic(Character, &AFPS_Charachter::OnKilledCharacter2Func);
	//}
	AController* CurrentController = GetController();
	auto Con = Cast<ANPC_AI_Controller>(CurrentController);
	if (Con)
	{
		Con->OwningPawn = this;
		Con->InitOnMatchStart();
		
	}
	GetWorldTimerManager().SetTimer(SetupTeamsTimer, this, &AFPS_Charachter::SetupTeams, DelaySetup, false);
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
		AController* CurrentController = GetController();
		if (CurrentController)
		{
			CurrentController->UnPossess();
			CurrentController->StopMovement();
			auto Con = Cast<ANPC_AI_Controller>(CurrentController);
			if (Con)
			{
				Con->DisableBehaviorTreeAndBlackboard();
			}
		//	CurrentController->Destroy();
		//	/*UE_LOG(LogTemp, Warning, TEXT("%s controller AI_CHAR"), *CurrentController->GetName());*/
		}
		OnDeathAddDeathToServer.Broadcast(this);
		GetCharacterMovement()->DisableMovement();
		//DetachFromControllerPendingDestroy();
		//UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));

		Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh3P->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_MAX);

		if (InstigatedBy)
		{
			auto InstigatorCharachter = InstigatedBy->GetCharacter();
			auto Causer = Cast<AFPS_Charachter>(InstigatorCharachter);
			if (Causer != NULL)
			{
				OnDeathNotify.Broadcast(this, Causer);
				//UE_LOG(LogTemp, Warning, TEXT("Health at DEAD notified"));
			}
		}
		HealthComponent->Deactivate();
		bDropWeaponOnDeath = true;
		DropWeapon(CurrentWeapon);
		//Mesh3P->SetAnimationMode(NULL);
		//Mesh1P->SetAnimationMode(NULL);
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

void AAI_Character::GetNewPathOnAllLoss_Implementation()
{

}

void AAI_Character::RotateHeadOnGuard()
{
	//HeadRotation = Mesh3P->GetBoneTransform()
	//FMath::RInterpTo(1)
}
