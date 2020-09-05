// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveItems.h"
#include "HealthComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Engine/LatentActionManager.h"
#include "GameFramework/DamageType.h"
#include "Perception/AIPerceptionSystem.h" 	
#include "GameFramework/Character.h"
#include "Perception/AISense_Hearing.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AExplosiveItems::AExplosiveItems()
{
	//UE_LOG(LogTemp, Warning, TEXT("BOOM"));
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = false;
	//MeshEx = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh_Comp_Bomb"));
	//MeshEx->SetCanEverAffectNavigation(false);
	MeshEx_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Display Mesh"));
	SetRootComponent(MeshEx_2);
	//Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	//PerceptionSystem = CreateDefaultSubobject<UAIPerceptionSystem>(TEXT("Hearing Perception"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));

	MeshEx_2->SetSimulatePhysics(true);
	RadialForceComp->Radius = 2500;
	RadialForceComp->bImpulseVelChange = false;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->ForceStrength = 10000;

	RadialForceComp->SetupAttachment(RootComponent);


	//RadialForceComp->AttachTo(MeshEx);

	//HearingPerception = CreateDefaultSubobject<UAISense_Hearing>(TEXT("Hearing Config"));
	//PerceptionSystem->RegisterSenseClass(*HearingPerception);

	//Sphere->SetVisibility(false);
	//Sphere->SetSphereRadius(ExplosionRadius / 2.f);
	//
	//Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	//Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	//Sphere->SetupAttachment(RootComponent);
	//
}

// Called when the game starts or when spawned
void AExplosiveItems::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AExplosiveItems::OnHealthChanged);
}



void AExplosiveItems::Explode()
{
	bIsDestroyed = true;

	//@TODO: Fix radial force direction and where mine goes after it blows up
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this->GetInstigatorController());
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, 1, 1, GetActorLocation(), 0, ExplosionRadius, 30, UDamageType::StaticClass(), IgnoreActors, this, this->GetInstigatorController());
	FVector BoostIntensity = UKismetMathLibrary::RandomUnitVector().Normalize() * FVector(400, 400, 400);
	MeshEx_2->AddImpulse(FVector::UpVector * 400, NAME_None, true);
	RadialForceComp->FireImpulse();
	if (OnDamagedShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), OnDamagedShake, GetActorLocation(), 0, ExplosionRadius + (ExplosionRadius / 2));
	}
	if(ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation(), 1, 1, 0, ExplosionAttenuation);
	SetLifeSpan(Lifespan);
}
	
void AExplosiveItems::OnHealthChanged(UHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDestroyed)
	{
		Explode();
	}
}
