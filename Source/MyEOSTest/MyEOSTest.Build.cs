// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyEOSTest : ModuleRules
{
	public MyEOSTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"Networking",
			"Sockets",
			//"OnlineSubsystemSteam",
			//"CommonUser",
		});

        PrivateDependencyModuleNames.AddRange(new string[] {
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"OnlineSubsystemNull",
			"OnlineSubsystemSteam",
        });

        //DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
    }
}
