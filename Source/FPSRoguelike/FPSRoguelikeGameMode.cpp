// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSRoguelikeGameMode.h"
#include "FPSRoguelikeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSRoguelikeGameMode::AFPSRoguelikeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
