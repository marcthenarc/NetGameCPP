// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "NetGameCPP.h"
#include "NetGameCPPGameMode.h"
#include "NetGameCPPHUD.h"

ANetGameCPPGameMode::ANetGameCPPGameMode(const class FObjectInitializer& oi)
	: Super(oi)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UClass> PlayerPawnBPClass(TEXT("Class'/Game/Blueprints/MyCharacter.MyCharacter_C'"));
	if (PlayerPawnBPClass.Object != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Object;

	}

	HUDClass = ANetGameCPPHUD::StaticClass();
}
