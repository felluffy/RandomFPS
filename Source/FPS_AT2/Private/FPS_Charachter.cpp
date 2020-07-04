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
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "WeaponBase.h" 	
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SynthComponent.h"
#include "FPS_AT2PlayerController.h"
#include "VoiceHttpSTTComponent.h"
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
	ZoomCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ZoomCameraComponent"));
	ZoomCameraComponent->SetupAttachment(FirstPersonCameraComponent);
	ZoomCameraComponent->Deactivate();
	//ZoomCameraComponent->Deactivate();

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
	VoiceComponent = CreateDefaultSubobject<UVoiceHttpSTTComponent>(TEXT("VoiceComp"));	
	//SynthComponent = CreateDefaultSubobject <USynthComponent>(TEXT("SynthComponent"));
	//AudioCaptureComponent = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
	PerceptionStimuliSourceComponent = CreateDefaultSubobject< UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	PerceptionStimuliSourceComponent->SetAutoActivate(true);



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
	HealthComp->SetTeam(TeamNumber);
	
	if(DefaultWeaponClasses.Num() > 0)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//SpawnInfo.Owner = this;
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[0], SpawnInfo);
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->AttachMeshToPawn();

		ZoomCameraComponent->SetWorldLocation(CurrentWeapon->GetFP_Gun()->GetSocketLocation("RifleZoomSocket"));
		ZoomCameraComponent->SetWorldRotation(CurrentWeapon->GetFP_Gun()->GetSocketRotation("RifleZoomSocket"));

		//ZoomCameraComponent->Attach
		
		Inventory.Add(CurrentWeapon);
		if (DefaultWeaponClasses.Num() > 1)
		{
			Inventory.Add(GetWorld()->SpawnActor<AWeaponBase>(DefaultWeaponClasses[1], SpawnInfo));
			Inventory[1]->SetOwningPawn(this);
		}
			//CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), WeaponAttachPoint);
		//UE_LOG(LogTemp, Warning, TEXT("%s - is the current weapon"), *CurrentWeapon->WeaponName.ToString());
		
	}

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

	ZoomCameraComponent->SetWorldLocation(CurrentWeapon->GetFP_Gun()->GetSocketLocation("RifleZoomSocket"));
	ZoomCameraComponent->SetWorldRotation(CurrentWeapon->GetFP_Gun()->GetSocketRotation("RifleZoomSocket"));
}

void AFPS_Charachter::DropWeapon(AWeaponBase* Weapon)
{
	//@TODO Drop weapon upon pressing F
	if(Inventory.Num() <= 1)
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
		//const FVector LinetraceStart = GetActorLocation();
		//const FVector LinetraceEnd = LinetraceStart + (CameraRotation.Vector() * DropWeaponMaxDistance);
		FVector LinetraceStart = Mesh1P->GetBoneLocation("hand_r");//GetFirstPersonCameraComponent()->GetComponentLocation();
		FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
		FVector LinetraceEnd = LinetraceStart + (NormalRotation * DefaultTraceDistance);

		DrawDebugSphere(GetWorld(), LinetraceEnd, 30, 2, FColor::Red, true, 5);
		
		FHitResult Hit;
		FCollisionQueryParams CollisionParams;
		GetWorld()->LineTraceSingleByChannel(Hit, LinetraceStart, LinetraceEnd, ECC_WorldDynamic, CollisionParams);
		if (Hit.bBlockingHit)
			SpawnLocation = Hit.ImpactPoint + (Hit.ImpactNormal * 200);
		else 
			SpawnLocation = LinetraceEnd;
		FActorSpawnParameters SpawnParams;
		AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(CurrentWeapon->GetClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		NewWeapon->DroppedOnWorld();
		NewWeapon->SetCurrentAmmo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetCurrentAmmoInMagazine());
		
	}
	Inventory.Remove(CurrentWeapon);
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
		//UE_LOG(LogTemp, Error, TEXT("%s"), *CurrentWeapon->WeaponName.ToString());
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
	UE_LOG(LogTemp, Warning, TEXT("on Next weapon"));
	UE_LOG(LogTemp, Warning, TEXT("%d - %d"), Inventory.Num(), DefaultWeaponClasses.Num());
	if (Inventory.Num() > 1)
	{
		int32 Index;
		Inventory.Find(CurrentWeapon, Index);
			Index = (Index + 1) % Inventory.Num();
		UE_LOG(LogTemp, Warning, TEXT("%d CRAZY FUDGE"), Index);
		EquipWeapon(Inventory[Index]);
	}
}

void AFPS_Charachter::PreviousWeapon()
{
	if (Inventory.Num() > 1)
	{
		int32 Index;
		Inventory.Find(CurrentWeapon, Index);
			Index = (Index - 1 + Inventory.Num()) % Inventory.Num();
		EquipWeapon(Inventory[Index]);
	}
}

bool AFPS_Charachter::CanFire()
{
	if(!isReloading)
		return true;
	return false;
}


void AFPS_Charachter::OrderAIAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("OrderAIAttack()"));
	if(PlayerController_AFPS2)
		PlayerController_AFPS2->OrderAttack(CurrentObjectHit.Location);
}

void AFPS_Charachter::OrderFollow()
{
	UE_LOG(LogTemp, Warning, TEXT("OrderFollow()"));
	if(PlayerController_AFPS2)
	PlayerController_AFPS2->OrderFollow();
}

void AFPS_Charachter::OrderGuard()
{
	FVector Loc = CurrentObjectHit.Location;
	UE_LOG(LogTemp, Warning, TEXT("OrderGuard()"));
	if(PlayerController_AFPS2)
		PlayerController_AFPS2->OrderGuard(Loc);

}

void AFPS_Charachter::OrderDismiss()
{
	UE_LOG(LogTemp, Warning, TEXT("OrderDismiss()"));
	if(PlayerController_AFPS2)
	PlayerController_AFPS2->OrderDismiss();
}


void AFPS_Charachter::OrderCallAssistance()
{
	UE_LOG(LogTemp, Warning, TEXT("OrderCallAssistance()"));
	if(PlayerController_AFPS2)
	PlayerController_AFPS2->CallAssistance();
}

void AFPS_Charachter::OrderAIGoToLocation(FVector Location, bool FromMap)
{
	UE_LOG(LogTemp, Warning, TEXT("OrderAIGoToLocation"))
		if (PlayerController_AFPS2)
			PlayerController_AFPS2->CommandMove(Location, FromMap);
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
	Crouch();
}

void AFPS_Charachter::EndCrouch()
{
	//CurrentWeapon->Equip();
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
		MovementComponent->MaxWalkSpeed = 450;
	else 
		MovementComponent->MaxWalkSpeed = 700;
	IsSprinting = true;
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
		PlayerController_AFPS2->RegisterBot(0);
	}
	//if (PlayerController_AFPS2)
	//{
	//	//line trace
	//	FHitResult Hit;
	//	FVector LinetraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	//	FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
	//	FVector LineTraceEnd = LinetraceStart + (NormalRotation * 5000);	//lline trace range is 500
	//	GetWorld()->LineTraceSingleByChannel(Hit, LinetraceStart, LineTraceEnd, ECC_Visibility);
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
		PlayerController_AFPS2->RegisterBot(1);
	}

}
void AFPS_Charachter::CommandBot_3()
{
	//AFPS_AT2PlayerController* PlayerController = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
		PlayerController_AFPS2->RegisterBot(2);
	}
}
void AFPS_Charachter::CommandBot_4()
{
	//AFPS_AT2PlayerController* PlayerController = Cast<AFPS_AT2PlayerController>(GetController());
	if (PlayerController_AFPS2)
	{
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
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), LinetraceStart, LineTraceEnd, {10,10,10}, { 0,0,0 }, ETraceTypeQuery::TraceTypeQuery1, false, { this }, EDrawDebugTrace::None, Hits, true);
	for (auto Hit : Hits)
	{
		AWeaponBase* Weapon = Cast<AWeaponBase>(Hit.GetActor());
		if (Weapon)
		{
			AddWeaponToInventory(Weapon);
			UE_LOG(LogTemp, Error, TEXT("Hit some weapon %s"),*(Weapon->WeaponName.ToString()));
		}
		UE_LOG(LogTemp, Error, TEXT("Hit some shit"));//, *(Hit.GetActor()->GetName()));
	}
}

void AFPS_Charachter::ZoomInToWeapon()
{
	//ZoomCameraComponent->Render;

}

void AFPS_Charachter::ZoomOutFromWeapon()
{

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
	UE_LOG(LogTemp, Warning, TEXT("%d num weapons"), Inventory.Num());
}




void AFPS_Charachter::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bInactive)
	{
		bInactive = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AController* CurrentController = Cast<AController>(GetController());
		if (CurrentController)
		{
			CurrentController->UnPossess();
			//CurrentController->StopMovement();
			CurrentController->Destroy();
		}

		Mesh3P->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh3P->SetAllBodiesSimulatePhysics(true);
		Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh3P->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_MAX);

		DetachFromControllerPendingDestroy();
		UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
		
	}
	else if(OnDamagedShake != NULL)
	{
		HealthDelta /= HealthComponent->GetMaxHealth();
		PlayerController_AFPS2->PlayerCameraManager->PlayCameraShake(OnDamagedShake, HealthDelta, ECameraAnimPlaySpace::CameraLocal);
	}
}


void AFPS_Charachter::OnVoiceRecognized(UVoiceHttpSTTComponent* STTComponent, float AccuracyScore, FString SentenceRetreived)
{
	RecognizedWord = SentenceRetreived.ToLower();
	if (RecognizedWord.Contains("follow"))
		this->OrderFollow();
	else if (RecognizedWord.Contains("defend"))
		this->OrderGuard();


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
	if (IsSprinting)
	{
		if (GetVelocity().IsZero() || FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) < .1f)
			MovementComponent->MaxWalkSpeed = 450;
		else
			MovementComponent->MaxWalkSpeed = 700;
	}
	if (OnMovmentShake != NULL)
	{
		auto CameraShakeScale = (this->GetVelocity().Size()) / 750.0f;
		if(CameraShakeScale > 0.0f)
			PlayerController_AFPS2->PlayerCameraManager->PlayCameraShake(OnMovmentShake, CameraShakeScale, ECameraAnimPlaySpace::CameraLocal);
	}

	//LINE TRACE AND STORE Value
	FVector LinetraceStart = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector NormalRotation = GetFirstPersonCameraComponent()->GetForwardVector();
	FVector LineTraceEnd = LinetraceStart + (NormalRotation * DefaultTraceDistance);
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), LinetraceStart, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery1, false, { this }, EDrawDebugTrace::None, CurrentObjectHit, true);

	

	//@TODO: pick up based on what's seen throiugh linetrace from eye to dropped equipment


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

	//PlayerInputComponent->BindAction("TEST_COMMAND_1", IE_Pressed, this, &AFPS_Charachter::CommandBot);
	PlayerInputComponent->BindAction("NextItem", IE_Pressed, this, &AFPS_Charachter::NextWeapon);
	PlayerInputComponent->BindAction("PreviousItem", IE_Pressed, this, &AFPS_Charachter::PreviousWeapon);
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


	//PlayerInputComponent->BindAxis("TEST_COMMAND_1", this, &AFPS_Charachter::CommandBot);

}