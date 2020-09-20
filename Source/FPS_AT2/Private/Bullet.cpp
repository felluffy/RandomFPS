// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include <string>
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstance.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/DamageType.h"

ABullet::ABullet()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetNotifyRigidBodyCollision( true );
	
	// set up a notification for when this component hits something blocking
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->bReturnMaterialOnMove = true;

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
	if (GetInstigator() != NULL)
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
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
		
// 		if(bIsExplosive)
// 			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), fDamage, 0, 

		ChangeDamageOnBone(fDamage, Hit.BoneName);
		if(fDamage > 0)
		{
			AController* Con = this->GetInstigator() == NULL ? NULL : this->GetInstigator()->GetController();
			UGameplayStatics::ApplyPointDamage(OtherActor, fDamage, GetActorLocation(), Hit, Con, this, UDamageType::StaticClass());		
			
			//UGameplayStatics::ReportDam
		}
		auto Prim = Cast<UPrimitiveComponent>(OtherActor);
		if (OtherComp->IsSimulatingPhysics())
			OtherComp->AddImpulseAtLocation(GetVelocity() / (Prim == NULL ? 1 : Prim->GetMass()), GetActorLocation());
		//@TODO Add decal on impact
		//UGameplayStatics::SpawnDecalAtLocation(GetWorld(), )
		
	}
	//DrawDebugSphere(GetWorld(), Hit.Location, 50, 6, FColor::White, false, 5.0f, 0, 5.0);
	PlayParticle(Hit);
	ApplyDecal(Hit);	
	Destroy();
}


void ABullet::PlayParticle(const FHitResult& Hit)
{
	
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
	FVector ScaleOut;
	UParticleSystem* ImpactParticle = GetSurfaceMat(SurfaceType, ImpactParticles, ScaleOut, true);
	if(ImpactParticle || DefaultParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle != NULL ? ImpactParticle : DefaultParticle, FTransform(FRotator(0, 0, 0), GetActorLocation(), ScaleOut));  //.3 for using grenade vfxs
}

void ABullet::ApplyDecal(const FHitResult& Hit)
{
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
	UE_LOG(LogTemp, Warning, TEXT("%d - surfacetype"), SurfaceType);
	FVector Scale;
	UMaterial* ImpactHole = GetSurfaceMat(SurfaceType, BulletImpactHoles, Scale);
	FRotator Rotation = UKismetMathLibrary::MakeRotFromX(-1 * Hit.ImpactNormal);
	Rotation.Roll += FMath::RandRange(-90, 90);
	if(ImpactHole || DefaultBulletHole)
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactHole != NULL ? ImpactHole : DefaultBulletHole, { 5, 15, 15 }, GetActorLocation(), Rotation, 30);


	//if (BulletImpactHoles.Num() > 0)
	//{
	//	uint8 Index = FMath::RandRange(0, BulletImpactHoles.Num());
	//	//UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletImpactHoles[Index], { 10,10, 10 }, GetActorLocation()); //GetActorLocation()->Rotation());
	//}
}

class UParticleSystem* ABullet::FindImpactParticle(EPhysicalSurface SurfaceType)
{
	UParticleSystem* ImpactParticle = nullptr;
	//switch (SurfaceType)
	//{
	//case SurfaceType_Default:
	//	ImpactParticle = ImpactParticles.IsValidIndex(0) ? ImpactParticles[0] : nullptr;//ImpactParticles.IsValidIndex(0) ? ImpactParticles[0] : nullptr;
	//	break;
	//case SurfaceType1:
	//	ImpactParticle = ImpactParticles.IsValidIndex(1) ? ImpactParticles[1] : nullptr;
	//	break;
	//case SurfaceType2:
	//	ImpactParticle = ImpactParticles.IsValidIndex(2) ? ImpactParticles[2] : nullptr;
	//	break;
	//case SurfaceType3:
	//	ImpactParticle = ImpactParticles.IsValidIndex(3) ? ImpactParticles[3] : nullptr;
	//	break;
	//case SurfaceType4:
	//	ImpactParticle = ImpactParticles.IsValidIndex(4) ? ImpactParticles[4] : nullptr;
	//	break;
	//case SurfaceType5:
	//	ImpactParticle = ImpactParticles.IsValidIndex(5) ? ImpactParticles[5] : nullptr;
	//	break;
	//case SurfaceType6:
	//	ImpactParticle = ImpactParticles.IsValidIndex(6) ? ImpactParticles[6] : nullptr;
	//	break;
	//case SurfaceType7:
	//	ImpactParticle = ImpactParticles.IsValidIndex(7) ? ImpactParticles[7] : nullptr;
	//	break;
	//case SurfaceType8:
	//	ImpactParticle = ImpactParticles.IsValidIndex(8) ? ImpactParticles[8] : nullptr;
	//	break;
	//case SurfaceType9:
	//	ImpactParticle = ImpactParticles.IsValidIndex(9) ? ImpactParticles[9] : nullptr;
	//	break;
	//default:
	//	ImpactParticle = DefaultParticle;
	//	break;
	//}
	return ImpactParticle;
}

class UMaterialInstance* ABullet::FindBulletImpactHole(EPhysicalSurface SurfaceType)
{
	UMaterialInstance* bla = nullptr;
	return bla;	
}

void ABullet::ChangeDamageOnBone(float &Damage, const FName &BoneName)
{
	//Use bones for now, change to uphysical material later
	std::string ss = std::string(TCHAR_TO_UTF8(*BoneName.ToString()));
	
	//Head
	if (ss.find("head") != std::string::npos)
	{
		Damage *= 3.5;
	}
	else if (ss.find("neck") != std::string::npos)
	{
		Damage *= 2.5;
	}
	else if (ss.find("hand") != std::string::npos)
	{
		Damage *= .75;
	}
	else if (ss.find("pelvis") != std::string::npos)
	{
		Damage *= 1.5;
	}
	else if (ss.find("lowerarm") != std::string::npos)
	{
		Damage *= .85;
	}
	else if (ss.find("upperarm") != std::string::npos)
	{
		Damage *= .95;
	}
	else if (ss.find("index") != std::string::npos)
	{
		Damage *= .5;
	}
	else if (ss.find("middle") != std::string::npos)
	{
		Damage *= .5;
	}
	else if (ss.find("pinky") != std::string::npos)
	{
		Damage *= .5;
	}
	else if (ss.find("thumb") != std::string::npos)
	{
		Damage *= .5;
	}
	else if (ss.find("ring") != std::string::npos)
	{
		Damage *= .5;
	}
	else if (ss.find("clavicle") != std::string::npos)
	{
		Damage *= 1.5;
	}
	else if (ss.find("thigh") != std::string::npos)
	{
		Damage *= 1.1;
	}
	else if (ss.find("foot") != std::string::npos)
	{
		Damage *= .85;
	}
	else if (ss.find("ball") != std::string::npos)
	{
		Damage *= .5;
	}
	return;

}
