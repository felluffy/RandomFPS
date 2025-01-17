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
#include "FPSSpectatorPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GrenadeExp.h"
#include "NPC_AI_Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "WeaponBase.h" 	
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SynthComponent.h"
#include "FPS_AT2PlayerController.h"
#include "AudioCaptureComponent.h"
#include "VoiceHttpSTTComponent.h"
#include "GameFramework/DamageType.h"
#include "WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"

//void AFPS_Charachter::OnKilledCharacter_Implementation(AFPS_Charachter* Killed, AFPS_Charachter* KilledBy)
//{
//	//Killed
//	UE_LOG(LogTemp, Error, TEXT("%s - killed by %s - is owner; %s"), *(Killed->GetName()), *(KilledBy->GetName()), *(this->GetName()));
//}

void AFPS_Charachter::OnKilledCharacter2Func_Implementation(class AFPS_Charachter* Killed, class AFPS_Charachter* KilledBy)
{
	UE_LOG(LogTemp, Error, TEXT("%s - killed by %s - is owner; %s"), *(Killed->GetName()), *(KilledBy->GetName()), *(this->GetName()));
}

void AFPS_Charachter::OnDeathAddDeathToServer_Func_Implementation(class AFPS_Charachter* CharacterDied)
{
	UE_LOG(LogTemp, Error, TEXT("%s - this guy died"), *(CharacterDied->GetName()));
}

// Sets default values
AFPS_Charachter::AFPS_Charachter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	Mesh3P->SetOwnerNoSee(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("CameraSpring");
	
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	//ZoomCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ZoomCameraComponent"));
	//ZoomCameraComponent->SetupAttachment(FirstPersonCameraComponent);
	//ZoomCameraComponent->Deactivate();
	//ZoomCameraComponent->Deactivate();

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)

	SpringArm->SetupAttachment(FirstPersonCameraComponent);
	SpringArm->bEnableCameraRotationLag = true;
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(SpringArm);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->SetAutoActivate(true);
	VoiceComponent = CreateDefaultSubobject<UVoiceHttpSTTComponent>(TEXT("VoiceComp"));
	//SynthComponent = CreateDefaultSubobject <USynthComponent>(TEXT("SynthComponent"));
	AudioCaptureComponent = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
	PerceptionStimuliSourceComponent = CreateDefaultSubobject< UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	PerceptionStimuliSourceComponent->SetAutoActivate(true);
	SetCanAffectNavigationGeneration(true, true);

	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();
	MovementComponent->MaxWalkSpeedCrouched = 200;
	DropWeaponMaxDistance = 200.0f;
	SprintingSpeedModifier = 3.0f;
	bInactive = false;
	Mesh3P->SetCollisionResponseToChannel(ECC_Camera, ECR_MAX);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_MAX);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AFPS_Charachter::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AFPS_Charachter::OnHealthChanged);
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
		//CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponAttachPoint);
	//UE_LOG(LogTemp, Warning, TEXT("%s - is the current weapon"), *CurrentWeapon->WeaponName.ToString());

	}
	DefaultWeaponClasses.Empty();

	auto it = Cast<AFPS_Charachter>(GetOwner());
	if (it)
	{
		USkeletalMeshComponent* PawnMesh1p = it->GetMeshComp(false);

		//CurrentWeapon->attacht
		//Mesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, it->WeaponAttachPoint);
	}

	PlayerController_AFPS2 = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
		VoiceComponent->OnRecognizedSpeech.AddDynamic(this, &AFPS_Charachter::OnVoiceRecognized);
		VoiceComponent->Activate();
		//UE_LOG(LogTemp, Warning, TEXT("%s - is the current controller"), *PlayerController_AFPS2->GetName());
	}
	ZoomOutFromWeapon();


}


void AFPS_Charachter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		if (Value < 0)
			Value /= 1.5;
		AddMovementInput(GetActorForwardVector(), Value);
	}

}

void AFPS_Charachter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		Value /= 1.25;
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

	if (Weapon && Weapon != CurrentWeapon)
	{
		SetCurrentWeapon(CurrentWeapon, Weapon);
	}
}

void AFPS_Charachter::SetCurrentWeapon(AWeaponBase* PreviousWeapon, AWeaponBase* NewWeapon)
{
	if (PreviousWeapon == nullptr || PreviousWeapon != NewWeapon)
	{
		CurrentWeapon = NewWeapon;
	}
	if (PreviousWeapon)
	{
		PreviousWeapon->Unequip();
	}
	NewWeapon->Equip();

	//ZoomCameraComponent->SetWorldLocation(CurrentWeapon->GetFP_Gun()->GetSocketLocation("RifleZoomSocket"));
	//ZoomCameraComponent->SetWorldRotation(CurrentWeapon->GetFP_Gun()->GetSocketRotation("RifleZoomSocket"));
}



void AFPS_Charachter::OnDropWeapon()
{
	DropWeapon(CurrentWeapon);
	UE_LOG(LogTemp, Warning, TEXT("DROP WEAPON"));
}

void AFPS_Charachter::RemoveWeapon(AWeaponBase* Weapon)
{
	if (!(Weapon))
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

void AFPS_Charachter::OnMovementPlaySound_Implementation()
{
	//MakeNoise(FireSoundScale, Owner, SpawnLocation, FireSoundRange);
}

void AFPS_Charachter::NextWeapon()
{
	if (Inventory.Num() > 1 && !CurrentWeapon->IsReloading())
	{
		if (IsAimingDown)
			ZoomOutFromWeapon();
		int32 Index;
		Inventory.Find(CurrentWeapon, Index);
		Index = (Index + 1) % Inventory.Num();
		EquipWeapon(Inventory[Index]);
	}
}

void AFPS_Charachter::PreviousWeapon()
{
	if (Inventory.Num() > 1 && !CurrentWeapon->IsReloading())
	{
		if (IsAimingDown)
			ZoomOutFromWeapon();
		int32 Index;
		Inventory.Find(CurrentWeapon, Index);
		Index = (Index - 1 + Inventory.Num()) % Inventory.Num();
		EquipWeapon(Inventory[Index]);
	}
}

bool AFPS_Charachter::CanFire()
{
	if (!isReloading)
		return true;
	return false;
}


void AFPS_Charachter::OrderAIAttack()
{
	if (PlayerController_AFPS2)
		PlayerController_AFPS2->OrderAttack(CurrentObjectHit.Location);
}

void AFPS_Charachter::OrderFollow()
{
	if (PlayerController_AFPS2)
		PlayerController_AFPS2->OrderFollow();
}

void AFPS_Charachter::OrderGuard()
{
	FVector Loc = CurrentObjectHit.Location;
	if (PlayerController_AFPS2)
		PlayerController_AFPS2->OrderGuard(Loc);

}

void AFPS_Charachter::OrderDismiss()
{
	if (PlayerController_AFPS2)
		PlayerController_AFPS2->OrderDismiss();
}


void AFPS_Charachter::OrderCallAssistance()
{
	if (PlayerController_AFPS2)
		PlayerController_AFPS2->CallAssistance();
}

void AFPS_Charachter::OrderAIGoToLocation(FVector Location, bool FromMap)
{
	UE_LOG(LogTemp, Warning, TEXT("OrderAIGoToLocation"))
	if (PlayerController_AFPS2)
		PlayerController_AFPS2->CommandMove(Location, FromMap);
}

class AFPS_AT2PlayerController* AFPS_Charachter::GetPlayerAFPSController() const
{
	return PlayerController_AFPS2;
}

void AFPS_Charachter::StartRecordingAudio_Implementation()
{
	//AudioCaptureComponent->Activate();
	//SynthComponent->start
	//AudioCaptureComponent->startr
}

void AFPS_Charachter::StopRecordingAudio_Implementation()
{

}

void AFPS_Charachter::BeginCrouch()
{
	//CurrentWeapon->Unequip();
	//IsCrouching = true;
	if (IsAimingDown)
		MovementComponent->MaxWalkSpeed = 150;
	RecalculateBaseEyeHeight();
	if (!IsCrouching && Mesh3P)
	{
		IsCrouching = true;
		Mesh3P->RelativeLocation.Z += 44;
	}
	//Mesh3P->SetRelativeLocation(Get)
	Crouch();
}

void AFPS_Charachter::EndCrouch()
{
	//CurrentWeapon->Equip();
	
	if (!IsAimingDown)
		MovementComponent->MaxWalkSpeed = 250;
	RecalculateBaseEyeHeight();
	if (Mesh3P && IsCrouching)
	{
		IsCrouching = false;
		Mesh3P->RelativeLocation.Z -= 44;
	}
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
	if (GetVelocity().IsZero() || FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) < .9f)
		MovementComponent->MaxWalkSpeed = 300, IsSprinting = false;
	else
		MovementComponent->MaxWalkSpeed = 700, IsSprinting = true;
	//IsSprinting = true;
	bShouldSprint = true;
	StopFire();
	bAllowedToFire = false;

}

void AFPS_Charachter::StopSprinting()
{
	MovementComponent->MaxWalkSpeed = 300;
	bShouldSprint = false;
	bAllowedToFire = true;
	IsSprinting = false;
}

void AFPS_Charachter::CommandBot()
{
	//Get all bots;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld)
	//UGameplayStatics::GetAllActorsOfClas

	if (PlayerController_AFPS2)
	{
		//line trace
		FHitResult Hit;
		FVector LinetraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
		FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
		FVector LineTraceEnd = LinetraceStart + (NormalRotation * 5000);	//lline trace range is 500
		GetWorld()->LineTraceSingleByChannel(Hit, LinetraceStart, LineTraceEnd, ECC_Visibility);
		FVector fv(0, 0, 0);
		if (Hit.bBlockingHit)
		{
			fv = Hit.Location;
			UE_LOG(LogTemp, Warning, TEXT("ETDTSKKM %f - %f - %f"), fv.X, fv.Y, fv.Z);
		}
		UE_LOG(LogTemp, Warning, TEXT("BLYAT"));

		PlayerController_AFPS2->CommandMove(fv, false);
	}
}

void AFPS_Charachter::CommandBot_1()
{
	//AFPS_AT2PlayerController* PlayerController = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterBot calling: 0"));
		PlayerController_AFPS2->RegisterBot(0);
	}
	//if (PlayerController_AFPS2)
	//{
	//	//line trace
	//	FHitResult Hit;
	//	FVector LinetraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	//	FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
	//	FVector LineTraceEnd = LinetraceStart + (NormalRotation * 5000);	//lline trace range is 500
	//	GetWorld()->fByChannel(Hit, LinetraceStart, LineTraceEnd, ECC_Visibility);
	//	FVector fv(0, 0, 0);
	//	if (Hit.bBlockingHit)
	//	{
	//		fv = Hit.Location;
	//		UE_LOG(LogTemp, Warning, TEXT("ETDTSKKM %f - %f - %f"), fv.X, fv.Y, fv.Z);
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("BLYAT"));

	//	PlayerController_AFPS2->CommandMove(fv);
	//}
}
void AFPS_Charachter::CommandBot_2()
{
	//AFPS_AT2PlayerController* PlayerController = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterBot calling: 1"));
		PlayerController_AFPS2->RegisterBot(1);
	}

}
void AFPS_Charachter::CommandBot_3()
{
	//AFPS_AT2PlayerController* PlayerController = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterBot calling: 2"));
		PlayerController_AFPS2->RegisterBot(2);
	}
}
void AFPS_Charachter::CommandBot_4()
{
	//AFPS_AT2PlayerController* PlayerController = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
		UE_LOG(LogTemp, Warning, TEXT("RegisterBot calling: 3"));
		PlayerController_AFPS2->RegisterBot(3);
	}
}

void AFPS_Charachter::OnPressedActionButton()
{
	float Distance = 300.0f;
	FVector LinetraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
	FVector LineTraceEnd = LinetraceStart + (NormalRotation * Distance);
	FVector HalfSize = FVector({ 10.0f });
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), LinetraceStart, LineTraceEnd, { 10,10,10 }, { 0,0,0 }, ETraceTypeQuery::TraceTypeQuery1, false, { this }, EDrawDebugTrace::None, Hits, true);
	for (auto Hit : Hits)
	{
		AWeaponBase* Weapon = Cast<AWeaponBase>(Hit.GetActor());
		if (Weapon)
		{
			AddWeaponToInventory(Weapon);
			//UE_LOG(LogTemp, Error, TEXT("Hit some weapon %s"),*(Weapon->WeaponName.ToString()));
		}
		//UE_LOG(LogTemp, Error, TEXT("Hit some shit"));//, *(Hit.GetActor()->GetName()));
	}
}

void AFPS_Charachter::ZoomInToWeapon()
{
	//ZoomCameraComponent->Render;
	SpringArm->bEnableCameraRotationLag = false;
	IsAimingDown = true;
	if (CurrentWeapon)
	{
		CurrentWeapon->bZoomed = true;

		FirstPersonCameraComponent->FieldOfView = FMath::FInterpTo(CurrentWeapon->GetWeaponFov(), DefaultFieldOfView, GetWorld()->GetDeltaSeconds(), .8f);
	}
	MovementComponent->MaxWalkSpeed = 200;

}

void AFPS_Charachter::ZoomOutFromWeapon()
{
	IsAimingDown = false;
	SpringArm->bEnableCameraRotationLag = true;
	if (CurrentWeapon)
	{
		CurrentWeapon->bZoomed = false;

		FirstPersonCameraComponent->FieldOfView = FMath::FInterpTo(DefaultFieldOfView, CurrentWeapon->GetWeaponFov(), GetWorld()->GetDeltaSeconds(), .8f);
	}
	MovementComponent->MaxWalkSpeed = 300;
}

void AFPS_Charachter::AddWeaponToInventory(AWeaponBase* Weapon)
{
	if (!Weapon)
		return;
	for (auto InInventoryWeapon : Inventory)
	{
		if (Weapon->WeaponName == InInventoryWeapon->WeaponName)
			return;
	}

	Weapon->SetOwningPawn(this);
	Inventory.AddUnique(Weapon);
}




void AFPS_Charachter::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	UE_LOG(LogTemp, Warning, TEXT("%s damage causer"), *(DamageCauser->GetName()));
	if (Health <= 0.0f && !bInactive)
	{
		bInactive = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AController* CurrentController = Cast<AFPS_AT2PlayerController>(GetController());
		OnDeathAddDeathToServer.Broadcast(this);
		if (CurrentController)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Healh: %d player con: %s"), Health, *Controller->GetName()));
			CurrentController->UnPossess();
			//PlayerController_AFPS2->UnPossess();
			CurrentController->StopMovement();
			//CurrentController->Destroy();
			//ADD  DEATH TO SERVER;

			DetachFromControllerPendingDestroy();
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			FTransform trans = this->GetTransform();

			UWorld* const World = GetWorld();
			auto bbb = World->SpawnActor<AFPSSpectatorPawn>(AFPSSpectatorPawn::StaticClass(), this->GetTransform(), ActorSpawnParams);
			CurrentController->Possess(bbb);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Healh: %d DEADAIFEIAT con: %s"), Health, *Controller->GetName()));
		Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh3P->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_MAX);
		//DropWeapon(CurrentWeapon)
		UE_LOG(LogTemp, Warning, TEXT("%s damage causer"), *(DamageCauser->GetName()));
		if (InstigatedBy)
		{
			auto Causer = Cast<AFPS_Charachter>(InstigatedBy->GetCharacter());
			if (Causer != NULL)
			{
				OnDeathNotify.Broadcast(this, Causer);
				UE_LOG(LogTemp, Warning, TEXT("Health at DEAD notified"));
			}
		}
		//UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
		HealthComponent->Deactivate();
		bDropWeaponOnDeath = true;
		DropWeapon(CurrentWeapon);
		//Mesh3P->SetAnimationMode(0);
		//Mesh1P->SetAnimationMode(EAnimationMode::Type::);


	}
	else if (OnDamagedShake != NULL)
	{
		HealthDelta /= HealthComponent->GetMaxHealth();
		PlayerController_AFPS2->PlayerCameraManager->PlayCameraShake(OnDamagedShake, HealthDelta, ECameraAnimPlaySpace::CameraLocal);
	}

}


void AFPS_Charachter::OnVoiceRecognized(UVoiceHttpSTTComponent* STTComponent, float AccuracyScore, FString SentenceRetreived)
{
	//@TODO Bad approach, change later to something quicker
	RecognizedWord = SentenceRetreived.ToLower();
	if (RecognizedWord.Contains("follow") || RecognizedWord.Contains("get behind") || RecognizedWord.Contains("come with") || RecognizedWord.Contains("stick to") || RecognizedWord.Contains("stick with") || RecognizedWord.Contains("walk behind") || RecognizedWord.Contains("cover me") || RecognizedWord.Contains("protect me") || RecognizedWord.Contains("on me"))
		this->OrderFollow();
	else if (RecognizedWord.Contains("defend") || RecognizedWord.Contains("guard") || RecognizedWord.Contains("cover") || RecognizedWord.Contains("safeguard") || RecognizedWord.Contains("patrol") || RecognizedWord.Contains("protect") || RecognizedWord.Contains("shield") || RecognizedWord.Contains("hold"))
		this->OrderGuard();
	else if (RecognizedWord.Contains("charge") || RecognizedWord.Contains("attack") || RecognizedWord.Contains("strike") || RecognizedWord.Contains("rush") || RecognizedWord.Contains("storm") || RecognizedWord.Contains("ambush") || RecognizedWord.Contains("assault"))
		this->OrderAIAttack();
	else if (RecognizedWord.Contains("assistance") || RecognizedWord.Contains("help") || RecognizedWord.Contains("support"))// || RecognizedWord.Contains("get behind") ||  RecognizedWord.Contains("get behind") || RecognizedWord.Contains("get behind") || RecognizedWord.Contains("get behind") || RecognizedWord.Contains("get behind"))
		this->OrderCallAssistance();
	else if (RecognizedWord.Contains("dismiss") || RecognizedWord.Contains("as you were") || RecognizedWord.Contains("get back"))
		this->OrderDismiss();

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Sentence retrieved: %s"), *RecognizedWord));

}

float AFPS_Charachter::PlayMontage(class UAnimMontage* MontageToPlay, float InPlayRate, FName StartSectionName)
{
	if (Mesh1P && Mesh3P && Mesh3P->AnimScriptInstance && Mesh1P->AnimScriptInstance && MontageToPlay)
	{
		return Mesh1P->AnimScriptInstance->Montage_Play(MontageToPlay, InPlayRate);
	}
	return 0.0f;
}

void AFPS_Charachter::StopAllMontage()
{
	if (Mesh1P && Mesh3P && Mesh3P->AnimScriptInstance && Mesh1P->AnimScriptInstance)
	{
		Mesh1P->AnimScriptInstance->Montage_Stop(0.0f);
		Mesh3P->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

bool AFPS_Charachter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor /*= NULL */) const
{
	static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;

	auto sockets = Mesh3P->GetAllSocketNames();
	//UE_LOG(LogTemp, Error, TEXT("Socket -Name checked: %d"), sockets.Num());

	for (int i = 0; i < sockets.Num(); i++)
	{
		FVector socketLocation = Mesh3P->GetSocketLocation(sockets[i]);
		//UE_LOG(LogTemp, Error, TEXT("Socket -Name checked: %s"), *(sockets[i].ToString()));
		const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, socketLocation
			, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_Pawn))
			, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));	
		//DrawDebugLine(GetWorld(), ObserverLocation, socketLocation, FColor::White, false, 1, 0, 2);
		NumberOfLoSChecksPerformed++;

		if (bHitSocket == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this))) {
			OutSeenLocation = socketLocation;
			OutSightStrength = 1;

			return true;
		}
	}

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, GetActorLocation()
		, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic))
		, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		//UE_LOG(LogTemp, Error, TEXT("true"));
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;

		return true;
	}

	//UE_LOG(LogTemp, Error, TEXT("false"));
	OutSightStrength = 0;
	return false;
}

bool AFPS_Charachter::IsFirstPerson() const
{
	return !bInactive && Controller && Controller->IsLocalPlayerController();
}

float AFPS_Charachter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	UAnimInstance * AnimInstance = (MeshToUse) ? MeshToUse->GetAnimInstance() : nullptr;
	if (AnimMontage && AnimInstance)
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}
	MeshToUse = NULL;
	return 0.f;
}

float AFPS_Charachter::PlayAnimMontageOnMesh(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/, bool UseFP /*= false*/)
{
	if (UseFP)
		MeshToUse = Mesh1P;
	else
		MeshToUse = Mesh3P;
	return PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

void AFPS_Charachter::SlideCharacter()
{
	if (!bIsSliding && GetVelocity().Size() > 500)
	{
		bIsSliding = true;
		GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &AFPS_Charachter::StopSliding, SlideTime, false);

		MovementComponent->AddImpulse(GetFirstPersonCameraComponent()->GetForwardVector() * 500);
		if (SlideAnimation != NULL)
		{
			//Play slide animations and change slide time to the anim's runtime;
		}
	}
}

void AFPS_Charachter::StopSliding()
{
	if (bIsSliding)
	{
		while (GetVelocity().Size() > 600);
		bIsSliding = false;
	}
}


void AFPS_Charachter::SetupTeams()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPS_Charachter::StaticClass(), FoundActors);
	for (auto &it : FoundActors)
	{
		auto Character = Cast<AFPS_Charachter>(it);
		if (Character && Character != this)
			this->OnDeathNotify.AddDynamic(Character, &AFPS_Charachter::OnKilledCharacter2Func);
		auto CurrentCon = this->GetController();
	
		if (CurrentCon && Cast<ANPC_AI_Controller>(CurrentCon) && Character->TeamNumber == this->TeamNumber)
		{
			auto Con = Cast<ANPC_AI_Controller>(Character->GetController());
			if (Con)
			{
				Con->RegisterredControllersPair.Add(std::make_pair(Cast<ANPC_AI_Controller>(CurrentCon), false));
			}
		}

	}
	//this->OnDeathNotify.AddDynamic(this, &AFPS_Charachter::OnKilledCharacter);
	this->OnDeathAddDeathToServer.AddDynamic(this, &AFPS_Charachter::OnDeathAddDeathToServer_Func);
}

void AFPS_Charachter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController != NULL)
	{
		//AFPS_AT2PlayerController ToSetController = Cast<AFPS_AT2PlayerController>(NewController);
		this->Controller = NewController;
		this->PlayerController_AFPS2 = Cast<AFPS_AT2PlayerController>(NewController);
		OnPossessAttachHud();
	}
}

void AFPS_Charachter::UnPossessed()
{
	this->PlayerController_AFPS2 = nullptr;
	Super::UnPossessed();
	OnUnpossessedRemoveHud();
}

void AFPS_Charachter::OnUnpossessedRemoveHud_Implementation()
{
	
}

void AFPS_Charachter::OnPossessAttachHud_Implementation()
{

}

void AFPS_Charachter::TriggerGrenade()
{
	if (GrenadeCount > 0 && GrenadeClass)
	{
		TimeSetToThrow = GetWorld()->GetRealTimeSeconds();
		StopFire();
		bIsAboutToUseGrenade = true;
	}
}

void AFPS_Charachter::ThrowGrenade()
{
	if (bIsAboutToUseGrenade)
	{

		float CurrentTime = (GetWorld()->GetRealTimeSeconds() - TimeSetToThrow) * 2;
		float CurrentMultiplier = FMath::Clamp(CurrentTime, MinSpeedToThrowGrenadein, MaxSpeedToThrowGrenadein);
		//Use animations to trigger later on
		auto SpawnPoint = GetActorLocation() + FVector(0, 0, 50) + (GetFirstPersonCameraComponent()->GetForwardVector() * 50);

		auto SpawnRot = GetFirstPersonCameraComponent()->GetComponentRotation();
		FActorSpawnParameters asp;
		asp.Instigator = this;
		asp.Owner = this;
		asp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		auto Nade = GetWorld()->SpawnActor<AGrenadeExp>(GrenadeClass, SpawnPoint, FRotator(0, 0, 0), asp);
		//Account char vel
		FVector Forward = GetFirstPersonCameraComponent()->GetForwardVector();
		FVector Impulse = GetActorLocation() + (Forward * (500.0f * CurrentMultiplier) + Forward * FVector::DotProduct(Forward, GetVelocity()));
		FVector OutLaunchVelocity;
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), SpawnPoint, Impulse, FLinearColor::Green, 5, 2);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Impulse, 50, 4, FLinearColor::White, 14, 5);
		bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc
		(
			this,
			OutLaunchVelocity,
			SpawnPoint,
			Impulse,
			0.0F,
			.8F
		);
		if (bHaveAimSolution)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Current multiplier, %f"), CurrentMultiplier));
		//FVector Vel = (Impulse + OutLaunchVelocity);// + (Forward * 5000.0f);
		Nade->SetProjectileVelocityOnThrow_2(OutLaunchVelocity, CurrentMultiplier);


		bIsAboutToUseGrenade = false;
		GrenadeCount--;

	}
}

void AFPS_Charachter::RemoveAllWidgetsFromViewPort_Implementation()
{

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
	if (bShouldSprint)
	{
		if (GetVelocity().IsZero() || FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) < .9f)
			MovementComponent->MaxWalkSpeed = 300, IsSprinting = false;
		else
			MovementComponent->MaxWalkSpeed = 700, IsSprinting = true;
	}
	if (OnMovmentShake != NULL)
	{
		auto CameraShakeScale = (this->GetVelocity().Size()) / 750.0f;
		if (CameraShakeScale > 0.0f && PlayerController_AFPS2)
			PlayerController_AFPS2->PlayerCameraManager->PlayCameraShake(OnMovmentShake, CameraShakeScale, ECameraAnimPlaySpace::CameraLocal);
	}

	//LINE TRACE AND STORE Value
	FVector LinetraceStart; //= //GetFirstPersonCameraComponent()->GetComponentLocation();
	FRotator NormalRotation; //=// GetFirstPersonCameraComponent()->GetForwardVector();
	Mesh3P->GetSocketWorldLocationAndRotation(EyeSocketName, LinetraceStart, NormalRotation);
	if (FirstPersonCameraComponent->IsActive())
	{
		NormalRotation = GetFirstPersonCameraComponent()->GetComponentRotation();
	}
	FVector LineTraceEnd = LinetraceStart + (NormalRotation.Vector() * DefaultTraceDistance);
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), LinetraceStart, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery1, false, { this }, EDrawDebugTrace::ForOneFrame, CurrentObjectHit, true);



	//@TODO: pick up based on what's seen throiugh linetrace from eye to dropped equipment


}

void AFPS_Charachter::DropWeapon(AWeaponBase* Weapon)
{
	//@TODO Drop weapon upon pressing F
	if (Inventory.Num() <= 1 && !bInactive)
		return;
	if (!Weapon)
		return;
	//if (Inventory.Contains(Weapon))
	//{
	//	Weapon->OnLeaveInventory();
	//	Inventory.RemoveSingle(Weapon);
	//}
	if (CurrentWeapon || bDropWeaponOnDeath)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		//GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		FVector SpawnLocation;
		/*FRotator SpawnRotation = CameraRotation;*/
		//line trace on where to drop
		//const FVector LinetraceStart = GetActorLocation();
		//const FVector LinetraceEnd = LinetraceStart + (CameraRotation.Vector() * DropWeaponMaxDistance);
		FVector LinetraceStart = Mesh3P->GetBoneLocation("hand_r");//GetFirstPersonCameraComponent()->GetComponentLocation();
		FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
		FVector LinetraceEnd = SpawnLocation = LinetraceStart + (NormalRotation * 20);
		//DrawDebugSphere(GetWorld(), LinetraceStart, 30, 8, FColor::Green, true, 5, 0, 2);
		//DrawDebugSphere(GetWorld(), LinetraceEnd, 30, 8, FColor::Red, true, 5, 0, 2);

		FHitResult Hit;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		//CollisionParams.
		GetWorld()->LineTraceSingleByChannel(Hit, LinetraceStart, LinetraceEnd, ECC_WorldDynamic, CollisionParams);
		//UKismetSystemLibrary::LineTraceSingle(GetWorld(), LinetraceStart, line)
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint + (Hit.ImpactNormal * 200);
			//	UE_LOG(LogTemp, Warning, TEXT("614"));
		}
		else
		{
			SpawnLocation = LinetraceEnd;
			//	UE_LOG(LogTemp, Warning, TEXT("619"));
		}
		//	DrawDebugSphere(GetWorld(), SpawnLocation, 30, 8, FColor::Blue, true, 5, 0, 2);
		FActorSpawnParameters SpawnParams;
		AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(CurrentWeapon->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

		NewWeapon->DroppedOnWorld();
		NewWeapon->SetCurrentAmmo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetCurrentAmmoInMagazine());

	}
	Inventory.Remove(CurrentWeapon);
	if (!bInactive)
		SetCurrentWeapon(CurrentWeapon, Inventory[0]);
	Weapon->Destroy();
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
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &AFPS_Charachter::Reload);
	PlayerInputComponent->BindAction("Suicide", IE_Pressed, this, &AFPS_Charachter::Suicide);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFPS_Charachter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFPS_Charachter::StopSprinting);
	PlayerInputComponent->BindAction("Drop_Item", IE_Pressed, this, &AFPS_Charachter::OnDropWeapon);
	PlayerInputComponent->BindAction("TEST_COMMAND_1", IE_Pressed, this, &AFPS_Charachter::CommandBot_1);
	PlayerInputComponent->BindAction("TEST_COMMAND_2", IE_Pressed, this, &AFPS_Charachter::CommandBot_2);
	PlayerInputComponent->BindAction("TEST_COMMAND_3", IE_Pressed, this, &AFPS_Charachter::CommandBot_3);
	PlayerInputComponent->BindAction("TEST_COMMAND_4", IE_Pressed, this, &AFPS_Charachter::CommandBot_4);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AFPS_Charachter::OrderAIAttack);
	PlayerInputComponent->BindAction("Guard", IE_Pressed, this, &AFPS_Charachter::OrderGuard);
	PlayerInputComponent->BindAction("Follow", IE_Pressed, this, &AFPS_Charachter::OrderFollow);
	PlayerInputComponent->BindAction("CallAssistance", IE_Pressed, this, &AFPS_Charachter::OrderCallAssistance);
	PlayerInputComponent->BindAction("Dismiss", IE_Pressed, this, &AFPS_Charachter::OrderDismiss);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AFPS_Charachter::OnPressedActionButton);
	PlayerInputComponent->BindAction("AudioCapture", IE_Pressed, this, &AFPS_Charachter::StartRecordingAudio_Implementation);
	PlayerInputComponent->BindAction("AudioCapture", IE_Released, this, &AFPS_Charachter::StopRecordingAudio_Implementation);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFPS_Charachter::ZoomInToWeapon);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFPS_Charachter::ZoomOutFromWeapon);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AFPS_Charachter::SlideCharacter);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AFPS_Charachter::TriggerGrenade);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &AFPS_Charachter::ThrowGrenade);

	//PlayerInputComponent->BindAction("TEST_COMMAND_1", IE_Pressed, this, &AFPS_Charachter::CommandBot);
	PlayerInputComponent->BindAction("NextItem", IE_Pressed, this, &AFPS_Charachter::NextWeapon);
	PlayerInputComponent->BindAction("PreviousItem", IE_Pressed, this, &AFPS_Charachter::PreviousWeapon);
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


	//PlayerInputComponent->BindAxis("TEST_COMMAND_1", this, &AFPS_Charachter::CommandBot);

}