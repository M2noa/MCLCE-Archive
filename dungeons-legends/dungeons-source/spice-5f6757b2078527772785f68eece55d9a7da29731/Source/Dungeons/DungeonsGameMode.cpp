// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include "DungeonsGameMode.h"
#include "DungeonsPlayerController.h"
#include "DungeonsCharacter.h"

ADungeonsGameMode::ADungeonsGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADungeonsPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}