// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TwinStickShooter : ModuleRules
{
	public TwinStickShooter(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils", "RHI", "RenderCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem" });
	}
}
