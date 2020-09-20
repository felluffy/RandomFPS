// Fill out your copyright notice in the Description page of Project Settings.


#include "DefendArea.h"

// Sets default values
ADefendArea::ADefendArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->SetRootComponent(RootComponent);
	Area = CreateDefaultSubobject<UBoxComponent>(TEXT("Area"));
	//Area->AttachToComponent(RootComponent);
	Area->SetupAttachment(RootComponent);
	Area->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Area->CastShadow = false;

}

// Called when the game starts or when spawned
void ADefendArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame


