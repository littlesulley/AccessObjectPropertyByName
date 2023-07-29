// Sulley. All rights reserved.

using UnrealBuildTool;

public class SulleyAccessObjectPropertyByName : ModuleRules
{
	public SulleyAccessObjectPropertyByName(ReadOnlyTargetRules Target) : base(Target)
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
				"Engine",
				"Slate",
				"SlateCore",
				"BlueprintGraph",
				"EditorStyle",
				"GraphEditor",
				"KismetCompiler",
				"UnrealEd",
				"SulleyAccessObjectPropertyByNameLib"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
			);
	}
}
