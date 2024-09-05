// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class LibreFPV : ModuleRules {
    public LibreFPV(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;	
		PublicDependencyModuleNames.AddRange(
            new string[] { 
                "Core", 
                "CoreUObject", 
                "Engine", 
                "InputCore",
                "LibreFPVEditor"
            }
        );
		PrivateDependencyModuleNames.AddRange(new string[] { 
            "Slate",
            "SlateCore",
            "NetCore",
            "JSON",
            "JsonUtilities"
        });
    }
}

