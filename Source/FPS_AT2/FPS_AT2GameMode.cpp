// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FPS_AT2GameMode.h"
#include "FPS_AT2HUD.h"
#include "FPS_AT2Character.h"
#include "UObject/ConstructorHelpers.h"

AFPS_AT2GameMode::AFPS_AT2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPS_AT2HUD::StaticClass();
}
