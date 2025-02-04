// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class Blueprint_CEditor : ModuleRules
{
	public Blueprint_CEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "RHI",
                "Slate",
                "SlateCore",
                "PropertyEditor",
                "UnrealEd",
                "EditorStyle",
                "ToolMenus",
                "Projects",
				"UMGEditor"
            }
            );
	}
}
