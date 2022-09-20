// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyEOSTestGameMode.h"
#include "MyEOSTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyEOSTestGameMode::AMyEOSTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
