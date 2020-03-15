// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Character.h"
#include "NPC_AI_Controller.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AAI_Character::AAI_Character()
{
	//Super();
	AIControllerClass = ANPC_AI_Controller::StaticClass();
	bUseControllerRotationYaw = true;

}

void AAI_Character::BeginPlay()
{
	Super::BeginPlay();	
	//HealthComp->OnHealthChanged.AddDynamic(this, &AAI_Character::OnHealthChanged);
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
			//CurrentController->StopMovement();
			CurrentController->Destroy();
		}

		DetachFromControllerPendingDestroy();
		UE_LOG(LogTemp, Warning, TEXT("Health at DEAD"));
	}
}
