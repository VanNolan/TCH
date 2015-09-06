// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "TwinStickShooter.h"
#include "TwinStickShooterGameMode.h"
#include "TwinStickShooterPawn.h"

ATwinStickShooterGameMode::ATwinStickShooterGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ATwinStickShooterPawn::StaticClass();
}

