// Copyright Epic Games, Inc. All Rights Reserved.

#include "MechanicsGameMode.h"
#include "MechanicsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMechanicsGameMode::AMechanicsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
