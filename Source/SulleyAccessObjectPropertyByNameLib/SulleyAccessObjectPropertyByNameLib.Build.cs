// Sulley. All rights reserved.

using UnrealBuildTool;

public class SulleyAccessObjectPropertyByNameLib : ModuleRules
{
	public SulleyAccessObjectPropertyByNameLib(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {

			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {

			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{

			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
			);
	}
}
