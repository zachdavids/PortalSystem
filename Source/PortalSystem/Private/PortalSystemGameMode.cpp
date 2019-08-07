// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PortalSystemGameMode.h"
#include "PortalSystemHUD.h"
#include "PortalSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

APortalSystemGameMode::APortalSystemGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = APortalSystemHUD::StaticClass();
}
