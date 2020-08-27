// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetWidget.h"

#include "Components/WidgetComponent.h"

// Sets default values
ATargetWidget::ATargetWidget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FrontFace = CreateDefaultSubobject<UWidgetComponent>(TEXT("Front Face"));
	BackFace = CreateDefaultSubobject<UWidgetComponent>(TEXT("Back Face"));
	//BackFace->SetupAttachment(FrontFace);
	//InitialLifeSpan = 5.0f;

	//BackFace
}

// Called when the game starts or when spawned
void ATargetWidget::BeginPlay()
{
	Super::BeginPlay();
	//GetWorldTimerManager().SetTimer(FadeInTimerHandle, &ATargetWidget::FadeIn, 0, false);
	//GetWorldTimerManager().SetTimer(FadeOuTimerHandle, &ATargetWidget::FadeOut, 5, false);
	
}

// Called every frame
void ATargetWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GetActorRotation().Yaw + 5;
	//FRotator rot = {}
	//this->AddActorWorldRotation({ 0, this->GetActorRotation().Yaw + (DeltaTime * 15), 0 });
	
}

void ATargetWidget::FadeOut()
{
	
	//FMath::FInterpTo(CurrentWeapon->GetWeaponFov(), DefaultFieldOfView, GetWorld()->GetDeltaSeconds(), .8f);
	
	//FLinearColor FLC = FrontFace->TintColorAndOpacity;
	//FrontFace->SetTintColorAndOpacity()
	//FrontFace->TintColorAndOpacity.A
}

void ATargetWidget::FadeIn()
{

}
