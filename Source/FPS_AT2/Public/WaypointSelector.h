// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "WaypointSelector.generated.h"

/**
 * 
 */
UCLASS()
class FPS_AT2_API UWaypointSelector : public UBTTaskNode
{
	GENERATED_BODY()

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

public:


	//variables
private:
	int index;
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		struct FBlackboardKeySelector Keys;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		struct FBlackboardKeySelector Waypoint;
public:
};
