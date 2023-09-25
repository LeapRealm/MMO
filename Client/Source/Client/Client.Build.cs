// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Client : ModuleRules
{
	public Client(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"Sockets",
			"Networking",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"ProtobufCore",
		});
		
		PrivateIncludePaths.AddRange(new string[]
		{
			"Client/",
			"Client/Game/",
			"Client/Network/",
			"Client/Network/Protocol/",
		});
	}
}
