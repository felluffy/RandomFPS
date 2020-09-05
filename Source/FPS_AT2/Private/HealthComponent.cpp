// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "FPS_Charachter.h"
#include "Perception/AISense_Damage.h"
#include "GameFramework/PlayerController.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//SetIsReplicated(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	//if (GetOwnerRole() == ROLE_Authority)
	//{
	auto Owner = GetOwner();
	if (Owner)
	{
		UE_LOG(LogTemp, Log, TEXT("Health at %s"), *Owner->GetName());
		//Owner->OnTakePointDamage.AddDynamic(this, &UHealthComponent::TakePointDamage);
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
	}
	//}

	CurrentHealth = MaxHealth;

}

void UHealthComponent::TakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (InstigatedBy)
	{
		if (!SpecialDamageType)
		{
			UE_LOG(LogTemp, Warning, TEXT("Any DAMAGE"));
			if (Damage <= 0.0f)//|| !InstigatedBy)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to cause damage"));
				return;
			}
			/*AFPS_Charachter* Damaged = Cast<AFPS_Charachter>(DamagedActor);
			AFPS_Charachter* Instigator = Cast<AFPS_Charachter>(InstigatedBy->GetPawn());
			if (Damaged && Instigator && Damaged->TeamNumber == Instigator->TeamNumber)
				Damage *= .1;*/

			if (IsFriendly(DamagedActor, Cast<AActor>(InstigatedBy->GetCharacter())))
			{
				Damage *= .3;
			}
			CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
			UE_LOG(LogTemp, Log, TEXT("Health at %f tan"), CurrentHealth);

			
			OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

		}
		else
			SpecialDamageType = false;
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Any DAMAGE"));
		if (Damage <= 0.0f)//|| !InstigatedBy)
		{	
			UE_LOG(LogTemp, Warning, TEXT("Failed to cause damage"));
			return;
		}
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		UE_LOG(LogTemp, Log, TEXT("Health at %f"), CurrentHealth);
		OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
		//OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
		
		//To relocate to character object
		//UAISense_Damage::ReportDamageEvent(DamagedActor, DamagedActor, InstigatedBy->GetCharacter(), Damage, InstigatedBy->GetCharacter()->GetActorLocation(), DamagedActor->GetActorLocation());
	}
}

void UHealthComponent::TakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	SpecialDamageType = true;
	UE_LOG(LogTemp, Warning, TEXT("POINT DAMAGE"));
	if (Damage <= 0.0f || !InstigatedBy)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Failed to cause damage"));
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("BONE: %s"), *BoneName->ToString());
	UE_LOG(LogTemp, Warning, TEXT("BONE: %s"), *(BoneName.ToString()));

	if (IsFriendly(DamagedActor, Cast<AActor>(InstigatedBy->GetCharacter())))
	{
		Damage *= .3;
	}
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (!ActorA || !ActorB)
	{		
		return true;
	}
	UHealthComponent* HealthCompActorA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	UHealthComponent* HealthCompActorB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));
	return HealthCompActorA->TeamNumber == HealthCompActorB->TeamNumber;
}

//void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(UHealthComponent, CurrentHealth);
//}