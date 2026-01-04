using UnrealBuildTool;

public class EzSaveGameEditor : ModuleRules
{
    public EzSaveGameEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "EzSaveGameRuntime",
                "Slate",
                "SlateCore",
                "InputCore",
                "EditorStyle",
                "LevelEditor",
                "WorkspaceMenuStructure",
                "ClassViewer",
                "PropertyEditor"
            }
        );
    }
}