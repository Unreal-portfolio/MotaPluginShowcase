using UnrealBuildTool;
using System.IO;

public class EzSaveGameRuntime : ModuleRules
{
	public EzSaveGameRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Json",
				"DeveloperSettings"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);

		// === Third Party Libraries ===
		string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "ThirdParty");
		
		// toml++ (header-only)
		PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "tomlplusplus"));
		
		// yaml-cpp
		string YamlCppPath = Path.Combine(ThirdPartyPath, "yaml-cpp");
		PublicIncludePaths.Add(Path.Combine(YamlCppPath, "include"));
		PrivateIncludePaths.Add(Path.Combine(YamlCppPath, "src"));
		
		// Agregar ruta para archivos copiados a Private/yaml-cpp
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private", "yaml-cpp"));
		
		// tinyxml2
		string TinyXml2Path = Path.Combine(ThirdPartyPath, "tinyxml2");
		PublicIncludePaths.Add(TinyXml2Path);
		
		// Definiciones
		PublicDefinitions.Add("YAML_CPP_STATIC_DEFINE");
		
		// Deshabilitar warnings de terceros
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
			
			// Deshabilitar warning C4702 (unreachable code) para yaml-cpp
			bEnableExceptions = true;
		}
		
		// Deshabilitar warnings específicos usando bEnableUndefinedIdentifierWarnings
		bEnableUndefinedIdentifierWarnings = false;
	}
}