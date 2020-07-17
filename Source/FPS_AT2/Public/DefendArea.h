// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"	
#include "Components/BoxComponent.h"
#include "DefendArea.generated.h"

UCLASS()
class FPS_AT2_API ADefendArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADefendArea();
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	class UBoxComponent* Area;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
