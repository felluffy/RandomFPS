// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Character.h"
#include "NPC_AI_Controller.h"

AAI_Character::AAI_Character()
{
	AIControllerClass = ANPC_AI_Controller::StaticClass();
	bUseControllerRotationYaw = true;
}