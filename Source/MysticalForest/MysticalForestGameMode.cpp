// Copyright Epic Games, Inc. All Rights Reserved.

#include "MysticalForestGameMode.h"
#include "MysticalForestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMysticalForestGameMode::AMysticalForestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
