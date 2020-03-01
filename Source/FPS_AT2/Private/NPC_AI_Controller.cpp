// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AI_Controller.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "AI_Character.h"

ANPC_AI_Controller::ANPC_AI_Controller()
{
	//BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	//BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	bWantsPlayerState = true;

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}


void ANPC_AI_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAI_Character* NPC = Cast<AAI_Character>(InPawn);
	//if(NPC && NPC->BotBehavior)
	//{
	//	if(NPC->BotBehavior->BlackboardAsset)
	//}
}