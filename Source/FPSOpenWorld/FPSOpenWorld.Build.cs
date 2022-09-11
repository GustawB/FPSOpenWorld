// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSOpenWorld : ModuleRules
{
	public FPSOpenWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "AIModule" });

	}
}
