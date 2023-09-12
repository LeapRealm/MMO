// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ProtobufCore : ModuleRules
{
	public ProtobufCore(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		
		PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
		
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Win64", "libprotobuf.lib"));
		}
        
		PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=1");
	}
}
