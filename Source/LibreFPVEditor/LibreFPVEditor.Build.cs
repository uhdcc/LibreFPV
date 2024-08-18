using UnrealBuildTool;

public class LibreFPVEditor: ModuleRules {
	public LibreFPVEditor(ReadOnlyTargetRules Target) : base(Target) {			
		PublicDependencyModuleNames.AddRange(new string[] {
	   });
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core", 
			"CoreUObject", 
			"Engine",
            "UnrealEd",
            "EditorScriptingUtilities"
        });				 
	}
}