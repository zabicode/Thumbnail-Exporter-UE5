// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThumbnailExportManager : ModuleRules
{
	public ThumbnailExportManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutitlity/Private",
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/UnrealEd/Public",
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Runtime/AssetRegistry/Public/AssetRegistry",
                //System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Developer/DesktopWidgets/Public/Widgets/Input",
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "Blutility",
                "EditorScriptingUtilities",
                "ContentBrowser",
				"InputCore",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",       // F�r Editor-spezifische Funktionen wie ThumbnailTools
				"AssetRegistry",  // F�r den Zugriff auf die Asset-Registry
				"ImageWrapper",   // F�r die Bildverarbeitungsfunktionen
				"ImageCore"      // Optional, je nach Bedarf f�r fortgeschrittene Bildmanipulation
			
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
