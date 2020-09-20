// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointSelector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC_AI_Controller.h"
#include "PatrolComponent.h"

EBTNodeResult::Type UWaypointSelector::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	index = Blackboard->GetValueAsInt(Keys.SelectedKeyName);

	//Get controlled points 
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();
	auto route = ControlledPawn->FindComponentByClass<UPatrolComponent>();
	if (!route)
	{
		UE_LOG(LogTemp, Warning, TEXT("Patrol route not  found"));
		return EBTNodeResult::Failed;

	}
	auto waypoints = route->GetWaypoints();
	if (waypoints.Num() == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s missing waypoints"), *(AIController->GetName()));
		return EBTNodeResult::Failed;
	}
	if(MoveTowardsClosestPoint)
	{
		float MinDistance = 99999999999.99f;
		int ind = 0;
		int toSetInd = 0;
		for (auto point : waypoints)
		{
			if ((ControlledPawn->GetActorLocation() - point->GetActorLocation()).Size()< MinDistance)
			{
				MinDistance = (ControlledPawn->GetActorLocation() - point->GetActorLocation()).Size();
				toSetInd = ind;
			}
			ind++;
		}
		index = toSetInd;
	}
	//set next waypoint
	Blackboard->SetValueAsObject(Waypoint.SelectedKeyName, waypoints[index]);

	//cycle index;
	index = (index + 1) % waypoints.Num();
	Blackboard->SetValueAsInt(Keys.SelectedKeyName, index);
	//UE_LOG(LogTemp, Warning, TEXT("waypointselector at %s - %d"), *(Blackboard->GetKeyName(2).ToString()), index);
	return EBTNodeResult::Succeeded;


}