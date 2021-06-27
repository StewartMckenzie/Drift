// Copyright Epic Games, Inc. All Rights Reserved.

#include "DriftGameMode.h"

#include "UObject/ConstructorHelpers.h"

ADriftGameMode::ADriftGameMode()
{


	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Drift/Characters/Hero/BP_HeroCharacter.BP_HeroCharacter_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
