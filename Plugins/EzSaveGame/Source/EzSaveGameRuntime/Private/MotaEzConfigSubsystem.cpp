#include "MotaEzConfigSubsystem.h"
#include "MotaEzSaveData.h"
#include "MotaEzSaveGameSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UMotaEzConfigSubsystem::UMotaEzConfigSubsystem()
{
}

void UMotaEzConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FString Error;
    if (!LoadInternal(Error))
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Failed to load initial config: %s"), *Error);
        CurrentSave = NewObject<UMotaEzSaveData>(this);
    }
}

void UMotaEzConfigSubsystem::Deinitialize()
{
    // Execute auto-save on exit if enabled
    ExecuteAutoSaveOnExit();
    
    Super::Deinitialize();
}

bool UMotaEzConfigSubsystem::ReloadFromDisk()
{
    FString Error;
    const bool bOk = LoadInternal(Error);
    if (!bOk)
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: ReloadFromDisk error: %s"), *Error);
    }
    return bOk;
}

bool UMotaEzConfigSubsystem::SaveToDisk()
{
    FString Error;
    const bool bOk = SaveInternal(Error);
    if (!bOk)
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: SaveToDisk error: %s"), *Error);
    }
    return bOk;
}


bool UMotaEzConfigSubsystem::LoadTextFile(const FString& FilePath, FString& OutText, FString& OutError) const
{
    OutError.Reset();
    OutText.Reset();

    if (!FPaths::FileExists(FilePath))
    {
        OutError = FString::Printf(TEXT("File does not exist: %s"), *FilePath);
        return false;
    }

    if (!FFileHelper::LoadFileToString(OutText, *FilePath))
    {
        OutError = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
        return false;
    }

    return true;
}

bool UMotaEzConfigSubsystem::SaveTextFile(const FString& FilePath, const FString& Text, FString& OutError) const
{
    OutError.Reset();

    const FString Dir = FPaths::GetPath(FilePath);
    if (!IFileManager::Get().DirectoryExists(*Dir))
    {
        if (!IFileManager::Get().MakeDirectory(*Dir, true))
        {
            OutError = FString::Printf(TEXT("Failed to create directory: %s"), *Dir);
            return false;
        }
    }

    if (!FFileHelper::SaveStringToFile(Text, *FilePath))
    {
        OutError = FString::Printf(TEXT("Failed to write file: %s"), *FilePath);
        return false;
    }

    return true;
}

bool UMotaEzConfigSubsystem::LoadInternal(FString& OutError)
{
    OutError.Reset();

    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        OutError = TEXT("Failed to get UMotaEzSaveGameSettings");
        return false;
    }

    const FString ConfigPath = Settings->GetConfigFilePath();
    const FString SchemaPath = Settings->GetSchemaFilePath();
    const EMotaEzConfigFormat Format = Settings->DefaultFormat;

    FMotaEzSaveSchema Schema;

    {
        FString SchemaText;
        FString LocalError;
        if (FPaths::FileExists(SchemaPath))
        {
            if (!LoadTextFile(SchemaPath, SchemaText, LocalError))
            {
                OutError = LocalError;
                return false;
            }

            if (!UMotaEzFormatSerializer::LoadSchema(Format, SchemaText, Schema, LocalError))
            {
                OutError = FString::Printf(TEXT("Error parsing schema: %s"), *LocalError);
                return false;
            }
        }
    }

    TMap<FString, FMotaEzValue> RawConfig;

    {
        if (FPaths::FileExists(ConfigPath))
        {
            FString ConfigText;
            FString LocalError;

            if (!LoadTextFile(ConfigPath, ConfigText, LocalError))
            {
                OutError = LocalError;
                return false;
            }

            if (!UMotaEzFormatSerializer::LoadConfig(Format, ConfigText, RawConfig, LocalError))
            {
                OutError = FString::Printf(TEXT("Error parsing config: %s"), *LocalError);
                return false;
            }
        }
    }

    if (!CurrentSave)
    {
        CurrentSave = NewObject<UMotaEzSaveData>(this);
    }

    BuildSaveDataFromSchemaAndConfig(Schema, RawConfig, CurrentSave.Get());

    return true;
}

bool UMotaEzConfigSubsystem::SaveInternal(FString& OutError)
{
    OutError.Reset();

    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        OutError = TEXT("Failed to get UMotaEzSaveGameSettings");
        return false;
    }

    if (!CurrentSave)
    {
        OutError = TEXT("CurrentSave is null, nothing to save");
        return false;
    }

    const FString ConfigPath = Settings->GetConfigFilePath();
    const EMotaEzConfigFormat Format = Settings->DefaultFormat;

    FString OutText;
    FString LocalError;

    if (!UMotaEzFormatSerializer::SaveConfig(Format, CurrentSave->Data, OutText, LocalError))
    {
        OutError = FString::Printf(TEXT("Error serializing config: %s"), *LocalError);
        return false;
    }

    if (!SaveTextFile(ConfigPath, OutText, LocalError))
    {
        OutError = LocalError;
        return false;
    }

    return true;
}

void UMotaEzConfigSubsystem::BuildSaveDataFromSchemaAndConfig(
    const FMotaEzSaveSchema& Schema,
    const TMap<FString, FMotaEzValue>& RawConfig,
    UMotaEzSaveData* OutSave
) const
{
    if (!OutSave)
    {
        return;
    }

    OutSave->Data.Reset();

    for (const auto& Pair : Schema.Fields)
    {
        const FString& Key = Pair.Key;
        const FMotaEzFieldDescriptor& Desc = Pair.Value;

        const FMotaEzValue* Existing = RawConfig.Find(Key);

        if (Existing)
        {
            OutSave->Data.Add(Key, *Existing);
        }
        else
        {
            FMotaEzValue NewVal;
            NewVal.Type = Desc.Type;
            NewVal.bIsArray = Desc.bIsArray;

            if (Desc.bIsArray)
            {
                NewVal.ArrayValues = Desc.DefaultArray;
            }
            else
            {
                NewVal.ScalarValue = Desc.DefaultValue;
            }

            OutSave->Data.Add(Key, NewVal);
        }
    }

    for (const auto& Pair : RawConfig)
    {
        const FString& Key = Pair.Key;
        if (!Schema.Fields.Contains(Key))
        {
            OutSave->Data.Add(Key, Pair.Value);
        }
    }
}

void UMotaEzConfigSubsystem::ExecuteAutoSaveOnExit()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        return;
    }

    // Check if auto-save on exit is enabled
    if (!Settings->bAutoSaveOnExit)
    {
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Auto-save on exit triggered"));

    // Save using QuickSave to the last used slot
    FString Error;
    if (SaveInternal(Error))
    {
        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Auto-save on exit completed successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Auto-save on exit failed: %s"), *Error);
    }
}

