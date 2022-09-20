// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyEOSTestEditorTarget : TargetRules
{
	public MyEOSTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("MyEOSTest");

		// This is used for touch screen development along with the "Unreal Remote 2" app
		//EnablePlugins.Add("RemoteSession");
	}
}
