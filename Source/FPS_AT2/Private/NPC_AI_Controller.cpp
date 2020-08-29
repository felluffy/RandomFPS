// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AI_Controller.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "AI_Character.h"

ANPC_AI_Controller::ANPC_AI_Controller()
{
	//BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	//BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	//bWantsPlayerState = true;

	//BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	//BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	
}


void ANPC_AI_Controller::UpdateControlRotation(float DeltaTime, bool bUpdatePawn /*= true*/)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator NewControlRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
			//NewControlRotation.Pitch += 10;
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = MyPawn->GetActorRotation();
		}

		// Don't pitch view unless looking at another pawn
		//if (NewControlRotation.Pitch != 0 && Cast<APawn>(GetFocusActor()) == nullptr)
		//{
		//	NewControlRotation.Pitch = 0.f;
		//}

		SetControlRotation(NewControlRotation);

		//if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}

void ANPC_AI_Controller::SetFocalPoint(FVector NewFocus, EAIFocusPriority::Type InPriority /*= EAIFocusPriority::Gameplay*/)
{
	// clear out existing
	ClearFocus(InPriority);

	// now set new focus
	if (InPriority >= FocusInformation.Priorities.Num())
	{
		FocusInformation.Priorities.SetNum(InPriority + 1);
	}

	DrawDebugSphere(GetWorld(), NewFocus, 10, 3, FColor::Silver, false, .2);

	FFocusKnowledge::FFocusItem& FocusItem = FocusInformation.Priorities[InPriority];
	FocusItem.Position = NewFocus;
}

void ANPC_AI_Controller::BeginPlay()
{
	Super::BeginPlay();
	OwningPawn = Cast<AAI_Character>(this->GetPawn());

}

void ANPC_AI_Controller::ScoreItems()
{

}

void ANPC_AI_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAI_Character* NPC = Cast<AAI_Character>(InPawn);
	if (NPC)
	{
		NPC->SetAAIController(this);
		
	}
	//if(NPC && NPC->BotBehavior)
	//{
	//	if(NPC->BotBehavior->BlackboardAsset)
	//}	
}

void ANPC_AI_Controller::ReOrderByDistance(TArray<AActor*> Actors, bool OrderByLower /*= true*/)
{
	if(!OwningPawn)
		return;
	auto AI_Location = OwningPawn->GetActorLocation();

	for (int i = 1; i < Actors.Num(); i++)
	{
		auto CurrentActor = Actors[i];
		auto CurrentDistance = (CurrentActor->GetActorLocation() - AI_Location).Size();
		int j = i - 1;
		//auto GetCurrent
		if(OrderByLower)
		{
			while (j >= 0 && (Actors[j]->GetActorLocation() - AI_Location).Size() > CurrentDistance)
			{
				Actors[j + 1] = Actors[j];
				j--;
			}
		}
		else
		{
			while (j >= 0 && (Actors[j]->GetActorLocation() - AI_Location).Size() < CurrentDistance)
			{
				Actors[j + 1] = Actors[j];
				j--;
			}
		}
		Actors[j+1] =  CurrentActor;
	}
}

int32 ANPC_AI_Controller::GetMaxInt(TArray<int32> Array)
{
	int32 max = INT_MIN;
	for (auto it : Array)
	{
		if (max < it)
		{
			max = it;
		}
	}
	return max;
}

bool ANPC_AI_Controller::ShouldCrouchBehindCover()
{
	//get hit from pawn, and a line trace from midddle of body to front, !blocked1 and blocked2
	FVector Eye;
	FRotator EyeRotation;
	//OwningPawn->
	FHitResult Hit1, Hit2;
	//FVector LineTraceEnd = Eye
	//UKismetSystemLibrary::LineTraceSingle(GetWorld(), )
	return true;
}
