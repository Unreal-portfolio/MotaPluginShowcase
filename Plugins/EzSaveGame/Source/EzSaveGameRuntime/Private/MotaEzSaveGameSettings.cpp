#include "MotaEzSaveGameSettings.h"
#include "Misc/Paths.h"

UMotaEzSaveGameSettings::UMotaEzSaveGameSettings()
{
    CategoryName = TEXT("Plugins");
    SectionName  = TEXT("EzSaveGame");

    DefaultFormat  = EMotaEzConfigFormat::Toml;
    SaveDirectory.Path = TEXT("EzSaveGame");

    ConfigFileName = TEXT("EzSaveConfig");
    SchemaFileName = TEXT("EzSaveSchema");
}

FString UMotaEzSaveGameSettings::GetExtensionForFormat() const
{
    switch (DefaultFormat)
    {
    case EMotaEzConfigFormat::Json: return TEXT("json");
    case EMotaEzConfigFormat::Yaml: return TEXT("yaml");
    case EMotaEzConfigFormat::Xml:  return TEXT("xml");
    case EMotaEzConfigFormat::Toml:
    default:
        return TEXT("toml");
    }
}

FString UMotaEzSaveGameSettings::GetConfigFilePath() const
{
    const FString BaseDir = FPaths::Combine(FPaths::ProjectSavedDir(), SaveDirectory.Path);
    const FString FileName = FString::Printf(TEXT("%s.%s"), *ConfigFileName, *GetExtensionForFormat());
    return FPaths::Combine(BaseDir, FileName);
}

FString UMotaEzSaveGameSettings::GetSchemaFilePath() const
{
    const FString BaseDir = FPaths::Combine(FPaths::ProjectSavedDir(), SaveDirectory.Path);
    const FString FileName = FString::Printf(TEXT("%s.%s"), *SchemaFileName, *GetExtensionForFormat());
    return FPaths::Combine(BaseDir, FileName);
}
