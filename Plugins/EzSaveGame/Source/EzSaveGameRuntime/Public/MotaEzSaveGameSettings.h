#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MotaEzFormatSerializer.h"
#include "MotaEzSaveGameSettings.generated.h"

/**
 * EzSaveGame plugin settings accessible from Project Settings.
 * Configures save file format, paths, encryption, and advanced options.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="EzSaveGame"))
class EZSAVEGAMERUNTIME_API UMotaEzSaveGameSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UMotaEzSaveGameSettings();

    UPROPERTY(Config, EditAnywhere, Category="Format", 
        meta=(DisplayName="File Format", 
              ToolTip="Choose the format for save files: TOML (recommended), JSON, YAML, or XML"))
    EMotaEzConfigFormat DefaultFormat;

    UPROPERTY(Config, EditAnywhere, Category="Paths",
        meta=(DisplayName="Save Directory",
              ToolTip="Directory inside Saved/ where configuration files will be stored",
              RelativeToGameContentDir))
    FDirectoryPath SaveDirectory;

    UPROPERTY(Config, EditAnywhere, Category="Paths",
        meta=(DisplayName="Config File Name",
              ToolTip="Base name for configuration file (without extension)"))
    FString ConfigFileName;

    UPROPERTY(Config, EditAnywhere, Category="Paths",
        meta=(DisplayName="Schema File Name",
              ToolTip="Base name for schema file (without extension)"))
    FString SchemaFileName;

    UPROPERTY(Config, EditAnywhere, Category="Advanced",
        meta=(DisplayName="Create Examples Automatically",
              ToolTip="If enabled, creates example files if they don't exist at startup"))
    bool bCreateExamplesOnInit = false;

    UPROPERTY(Config, EditAnywhere, Category="Advanced",
        meta=(DisplayName="Auto-Save on Exit",
              ToolTip="Automatically saves data when the game closes"))
    bool bAutoSaveOnExit = false;

    UPROPERTY(Config, EditAnywhere, Category="Advanced",
        meta=(DisplayName="Verbose Logging",
              ToolTip="Shows detailed load/save information in the Output Log"))
    bool bVerboseLogging = false;

    UPROPERTY(Config, EditAnywhere, Category="Security",
        meta=(DisplayName="Enable Encryption",
              ToolTip="Encrypts save files in packaged game. Always disabled in editor for easier debugging."))
    bool bEnableEncryption = false;

    UPROPERTY(Config, EditAnywhere, Category="Security",
        meta=(DisplayName="Encryption Key",
              ToolTip="Custom encryption key. If empty, will be auto-generated.",
              EditCondition="bEnableEncryption"))
    FString EncryptionKey;

    /**
     * Gets the full file path for the config file.
     * @return Absolute path to config file
     */
    FString GetConfigFilePath() const;
    
    /**
     * Gets the full file path for the schema file.
     * @return Absolute path to schema file
     */
    FString GetSchemaFilePath() const;

    /**
     * Gets the file extension for the current format.
     * @return Extension string (.toml, .json, .yaml, or .xml)
     */
    FString GetExtensionForFormat() const;

#if WITH_EDITOR
    virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
    virtual FText GetSectionText() const override { return FText::FromString(TEXT("EzSaveGame")); }
    virtual FText GetSectionDescription() const override 
    { 
        return FText::FromString(TEXT("Configure EzSaveGame save system - Supports TOML, JSON, YAML and XML")); 
    }
#endif
};
