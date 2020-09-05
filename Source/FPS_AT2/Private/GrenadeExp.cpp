// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeExp.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Materials/MaterialInstance.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"

AGrenadeExp::AGrenadeExp()
{

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->bRotationFollowsVelocity = true;
	//ProjectileMovement->InitialSpeed = 2000;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->ProjectileGravityScale = 0.1f;
	PointLight->SetupAttachment(RootComponent);
}

void AGrenadeExp::BeginPlay()
{
	Super::BeginPlay();
	PointLight->ToggleVisibility(false);
}

void AGrenadeExp::Explode()
{
	FVector LineTraceStart = GetActorLocation() + FVector(0,0,100);
	FVector LineTraceEnd = LineTraceStart - FVector(0,0, 100);
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceEnd, ECC_MAX);
	if (Hit.bBlockingHit)
	{
		ApplyDecal(Hit);
		ApplyParticle(Hit);
	}
	else
	{
		//defualts 
		ImpactDecal = ImpactDecals.IsValidIndex(0) ? ImpactDecals[0] : nullptr;
		if (ImpactDecal)
		{
			FRotator Rotation = UKismetMathLibrary::MakeRotFromX(-1 * Hit.ImpactNormal);
			
			float size = FMath::FRandRange(50, 150);
			float rotsize = FMath::FRandRange(0, 90);
			Rotation.Roll += rotsize;
			Rotation.Yaw += rotsize;
			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecal, { size, size, size }, GetActorLocation(), Rotation, 30);
		}
		ImpactParticle = ImpactParticles.IsValidIndex(0) ? ImpactParticles[0] : nullptr;
		if(ImpactParticle)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, FTransform(GetActorLocation()));
	}

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this->GetInstigatorController());
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 0, GetActorLocation(), 0, RadialForceComp->Radius, 10, UDamageType::StaticClass(), IgnoreActors, this, this->GetInstigatorController());
	FVector BoostIntensity = UKismetMathLibrary::RandomUnitVector().Normalize() * FVector(400, 400, 400);
	MeshEx_2->AddImpulse(FVector::UpVector * 400, NAME_None, true);
	RadialForceComp->FireImpulse();
	if (OnDamagedShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), OnDamagedShake, GetActorLocation(), 0, ExplosionRadius + (ExplosionRadius / 2));
	}
	if (ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation(), 1, 1, 0, ExplosionAttenuation);
	

	MeshEx_2->SetHiddenInGame(true);
	PointLight->ToggleVisibility(false);
	GetWorldTimerManager().SetTimer(LightHandle, this, &AGrenadeExp::DisableLight, .5, false);
	SetActorHiddenInGame(true);
	SetLifeSpan(Lifespan);
	
}

void AGrenadeExp::Throw()
{			
	float FuseMod = FMath::FRandRange(FuseTime - 1, FuseTime + 1);
	GetWorldTimerManager().SetTimer(FuseHandle, this, &AGrenadeExp::Explode, FuseMod, false);
}

void AGrenadeExp::DisableLight()
{
	PointLight->ToggleVisibility(false);
}

void AGrenadeExp::ApplyParticle(const FHitResult& Hit)
{
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
	UParticleSystem* ImpactParticle_ = GetSurfaceMat(SurfaceType, ImpactParticles);
	if (ImpactParticle_)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle_, FTransform(GetActorLocation()));
}

void AGrenadeExp::ApplyDecal(const FHitResult& Hit)
{
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
	UE_LOG(LogTemp, Warning, TEXT("%d - surfacetype"), SurfaceType);
	UMaterial* ImpactHole = GetSurfaceMat(SurfaceType, ImpactDecals);
	FRotator Rotation = UKismetMathLibrary::MakeRotFromX(-1 * Hit.ImpactNormal);
	float size = FMath::FRandRange(100, 250);
	float rotsize = FMath::FRandRange(0, 90);
	Rotation.Roll += rotsize;
	Rotation.Yaw += rotsize;
	//Rotation.Roll += FMath::RandRange(-90, 90);
	if (ImpactHole)
	{
		auto aa = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactHole, { size,size,size }, GetActorLocation(), Rotation, 30);
		aa->SetFadeOut(5, 20, true); 
	}
}

void AGrenadeExp::SetProjectileVelocityOnThrow_2_Implementation(FVector AnyImpulse, float MultiPlier)
{
	FName n = FName(*AnyImpulse.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Thrown from c++, %s"), *AnyImpulse.ToString()));
	ProjectileMovement->SetVelocityInLocalSpace(AnyImpulse * MultiPlier);
	ProjectileMovement->Activate();
	Throw();
}

void AGrenadeExp::SetProjectileVelocityOnThrow_Implementation(FVector AnyImpulse)
{

}
