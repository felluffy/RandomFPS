// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/DamageType.h"

ABullet::ABullet()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
			// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// Set as root component
	RootComponent = CollisionComp;
	MeshComp->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 30000.f;
	ProjectileMovement->MaxSpeed = 30000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.1f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	
	//Weapon stats, change it to a class later
	//fDamage = Damage;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (TrailParticleComp)
	{
		TrailParticleComp->Deactivate();
	}

	// Only add impulse and destroy projectile if we hit a physics
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
	//if ((OtherActor != NULL) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	if ((OtherActor != NULL) && (OtherComp != NULL))
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());
		FHitResult HitInfo;
// 		if(bIsExplosive)
// 			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), fDamage, 0, 
		if(fDamage > 0)
			UGameplayStatics::ApplyPointDamage(OtherActor, fDamage, GetActorLocation(), HitInfo, NULL, this, UDamageType::StaticClass());		
		
		
		//@TODO Add decal on impact
		//UGameplayStatics::SpawnDecalAtLocation(GetWorld(), )
		
	}
	//DrawDebugSphere(GetWorld(), Hit.Location, 50, 6, FColor::White, false, 5.0f, 0, 5.0);
	PlayParticle();
	ApplyDecal();
	Destroy();
}


void ABullet::PlayParticle()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, FTransform(GetActorLocation()));
}

void ABullet::ApplyDecal()
{
	if (BulletImpactHoles.Num() > 0)
	{
		uint8 Index = FMath::RandRange(0, BulletImpactHoles.Num());
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletImpactHoles[Index], { 10,10, 10 }, GetActorLocation()); //GetActorLocation()->Rotation());
	}
}