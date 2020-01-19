// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Charachter.h"
#include "HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/InputSettings.h"
// Sets default values
AFPS_Charachter::AFPS_Charachter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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


}

// Called when the game starts or when spawned
void AFPS_Charachter::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AFPS_Charachter::OnHealthChanged);
}


void AFPS_Charachter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
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

void AFPS_Charachter::StartFire()
{

}

void AFPS_Charachter::StopFire()
{

}

void AFPS_Charachter::BeginCrouch()
{
	Crouch();
}

void AFPS_Charachter::EndCrouch()
{
	UnCrouch();
}

void AFPS_Charachter::Suicide()
{
	UGameplayStatics::ApplyDamage(this, 1000, GetController()->GetInstigatorController(), this, UDamageType::StaticClass());
}

// Called to bind functionality to input
void AFPS_Charachter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPS_Charachter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPS_Charachter::EndCrouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPS_Charachter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPS_Charachter::StopFire);
	PlayerInputComponent->BindAction("Suicide", IE_Pressed, this, &AFPS_Charachter::Suicide);

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
