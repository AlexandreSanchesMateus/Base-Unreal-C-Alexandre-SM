// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IntroC_project : ModuleRules
{
	public IntroC_project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
