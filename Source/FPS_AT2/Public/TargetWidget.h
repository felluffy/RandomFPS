// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetWidget.generated.h"

UCLASS()
class FPS_AT2_API ATargetWidget : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Properties)
	class UWidgetComponent* FrontFace;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Properties)
	class UWidgetComponent* BackFace;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Properties)
	float DespawnTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Properties)
	float FadeInTimer;
	FTimerHandle FadeInTimerHandle;
	FTimerHandle FadeOuTimerHandle;



public:	
	// Sets default values for this actor's properties
	ATargetWidget();
	void FadeOut();
	void FadeIn();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Properties)
	float ScaleDownLimit;
};
