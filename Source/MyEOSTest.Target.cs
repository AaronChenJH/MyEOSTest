// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyEOSTestTarget : TargetRules
{
	public MyEOSTestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		//// ʹ��stema��api
		//bUsesSteam = true;

		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("MyEOSTest");
        
    }
}
