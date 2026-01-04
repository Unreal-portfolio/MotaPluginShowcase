#include "MotaEzSlotManager.h"
#include "MotaEzConfigSubsystem.h"
#include "MotaEzSaveData.h"
#include "MotaEzSaveGameSettings.h"
#include "MotaEzFormatSerializer.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"
#include "ImageUtils.h"
#include "Engine/GameViewportClient.h"

const FString UMotaEzSlotManager::LastUsedSlotKey = TEXT("LastUsedSlot");
const int32 UMotaEzSlotManager::AutoSaveSlotIndex = -1;

static UMotaEzConfigSubsystem* GetSubsystem(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;
    
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World) return nullptr;
    
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance) return nullptr;
    
    return GameInstance->GetSubsystem<UMotaEzConfigSubsystem>();
}

FString UMotaEzSlotManager::GetSlotConfigPath(int32 SlotIndex)
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings) return TEXT("");

    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), Settings->SaveDirectory.Path);
    FString Extension = Settings->GetExtensionForFormat();
    
    FString SlotName = (SlotIndex == AutoSaveSlotIndex) ? TEXT("AutoSave") : FString::Printf(TEXT("Slot%d"), SlotIndex);
    
    // Ensure extension starts with a dot
    if (!Extension.IsEmpty() && !Extension.StartsWith(TEXT(".")))
    {
        Extension = TEXT(".") + Extension;
    }
    
    return FPaths::Combine(SaveDir, FString::Printf(TEXT("%s%s"), *SlotName, *Extension));
}

FString UMotaEzSlotManager::GetSlotMetadataPath(int32 SlotIndex)
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings) return TEXT("");

    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), Settings->SaveDirectory.Path);
    FString SlotName = (SlotIndex == AutoSaveSlotIndex) ? TEXT("AutoSave") : FString::Printf(TEXT("Slot%d"), SlotIndex);
    
    return FPaths::Combine(SaveDir, FString::Printf(TEXT("%s_Metadata.json"), *SlotName));
}

bool UMotaEzSlotManager::SaveToSlot(const UObject* WorldContextObject, int32 SlotIndex, FString& OutError)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        OutError = TEXT("Failed to get subsystem");
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: SaveToSlot - Failed to get subsystem"));
        return false;
    }

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData)
    {
        OutError = TEXT("No data to save");
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: SaveToSlot - No data to save"));
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: SaveToSlot %d - Saving %d keys"), SlotIndex, SaveData->Data.Num());

    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        OutError = TEXT("Failed to get settings");
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: SaveToSlot - Failed to get settings"));
        return false;
    }

    FString ConfigText;
    if (!UMotaEzFormatSerializer::SaveConfig(Settings->DefaultFormat, SaveData->Data, ConfigText, OutError))
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: SaveToSlot - Serialization failed: %s"), *OutError);
        return false;
    }

    FString ConfigPath = GetSlotConfigPath(SlotIndex);
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: SaveToSlot - Writing to: %s"), *ConfigPath);
    
    if (!FFileHelper::SaveStringToFile(ConfigText, *ConfigPath))
    {
        OutError = FString::Printf(TEXT("Failed to write file: %s"), *ConfigPath);
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: SaveToSlot - %s"), *OutError);
        return false;
    }

    FMotaEzSlotInfo SlotInfo;
    SlotInfo.SlotIndex = SlotIndex;
    SlotInfo.LastModified = FDateTime::Now();
    SlotInfo.bIsOccupied = true;
    
    FMotaEzSlotInfo ExistingInfo;
    if (LoadSlotMetadata(SlotIndex, ExistingInfo))
    {
        SlotInfo.SlotName = ExistingInfo.SlotName.IsEmpty() ? FString::Printf(TEXT("Save %d"), SlotIndex) : ExistingInfo.SlotName;
        SlotInfo.PlayTime = ExistingInfo.PlayTime;
        SlotInfo.PlayerLevel = ExistingInfo.PlayerLevel;
        SlotInfo.CharacterLevel = ExistingInfo.CharacterLevel;
        SlotInfo.ScreenshotPath = ExistingInfo.ScreenshotPath;
    }
    else
    {
        SlotInfo.SlotName = FString::Printf(TEXT("Save %d"), SlotIndex);
    }

    SaveSlotMetadata(SlotIndex, SlotInfo);

    // Update LastUsedSlot in both memory and global config
    if (SlotIndex != AutoSaveSlotIndex)
    {
        SaveData->SetInt(LastUsedSlotKey, SlotIndex);
        
        // Save to global config for persistence
        FString GlobalConfigPath = Settings->GetConfigFilePath();
        TMap<FString, FMotaEzValue> GlobalData;
        
        // Load existing global config if it exists
        if (FPaths::FileExists(GlobalConfigPath))
        {
            FString GlobalConfigText;
            if (FFileHelper::LoadFileToString(GlobalConfigText, *GlobalConfigPath))
            {
                FString Error;
                UMotaEzFormatSerializer::LoadConfig(Settings->DefaultFormat, GlobalConfigText, GlobalData, Error);
            }
        }
        
        // Update LastUsedSlot in global config
        FMotaEzValue LastSlotValue;
        LastSlotValue.Type = EMotaEzFieldType::Int;
        LastSlotValue.ScalarValue = FString::FromInt(SlotIndex);
        GlobalData.Add(LastUsedSlotKey, LastSlotValue);
        
        // Save global config
        FString GlobalConfigText;
        FString Error;
        if (UMotaEzFormatSerializer::SaveConfig(Settings->DefaultFormat, GlobalData, GlobalConfigText, Error))
        {
            FFileHelper::SaveStringToFile(GlobalConfigText, *GlobalConfigPath);
            UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Updated LastUsedSlot to %d in global config"), SlotIndex);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Saved to slot %d: %s"), SlotIndex, *ConfigPath);
    return true;
}

bool UMotaEzSlotManager::LoadFromSlot(const UObject* WorldContextObject, int32 SlotIndex, FString& OutError)
{
    FString ConfigPath = GetSlotConfigPath(SlotIndex);
    
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: LoadFromSlot %d - Checking path: %s"), SlotIndex, *ConfigPath);
    
    if (!IsSlotOccupied(SlotIndex))
    {
        OutError = FString::Printf(TEXT("Slot %d is empty - File does not exist: %s"), SlotIndex, *ConfigPath);
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: %s"), *OutError);
        return false;
    }

    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem)
    {
        OutError = TEXT("Failed to get subsystem");
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: LoadFromSlot - Failed to get subsystem"));
        return false;
    }

    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        OutError = TEXT("Failed to get settings");
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: LoadFromSlot - Failed to get settings"));
        return false;
    }

    FString ConfigText;
    if (!FFileHelper::LoadFileToString(ConfigText, *ConfigPath))
    {
        OutError = FString::Printf(TEXT("Failed to read file: %s"), *ConfigPath);
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: LoadFromSlot - %s"), *OutError);
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: LoadFromSlot - File loaded, size: %d bytes"), ConfigText.Len());

    TMap<FString, FMotaEzValue> LoadedData;
    if (!UMotaEzFormatSerializer::LoadConfig(Settings->DefaultFormat, ConfigText, LoadedData, OutError))
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: LoadFromSlot - Deserialization failed: %s"), *OutError);
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: LoadFromSlot - Loaded %d keys"), LoadedData.Num());

    // Log all loaded keys for debugging
    for (const auto& Pair : LoadedData)
    {
        UE_LOG(LogTemp, Verbose, TEXT("EzSaveGame: LoadFromSlot - Key: %s"), *Pair.Key);
    }

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (SaveData)
    {
        SaveData->Data = LoadedData;
        
        if (SlotIndex != AutoSaveSlotIndex)
        {
            SaveData->SetInt(LastUsedSlotKey, SlotIndex);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Loaded from slot %d: %s"), SlotIndex, *ConfigPath);
    return true;
}

bool UMotaEzSlotManager::DeleteSlot(int32 SlotIndex)
{
    FString ConfigPath = GetSlotConfigPath(SlotIndex);
    FString MetadataPath = GetSlotMetadataPath(SlotIndex);
    FString ScreenshotPath = GetSlotScreenshotPath(SlotIndex);

    bool bSuccess = true;

    if (FPaths::FileExists(ConfigPath))
    {
        bSuccess &= IFileManager::Get().Delete(*ConfigPath);
    }

    if (FPaths::FileExists(MetadataPath))
    {
        bSuccess &= IFileManager::Get().Delete(*MetadataPath);
    }

    if (FPaths::FileExists(ScreenshotPath))
    {
        bSuccess &= IFileManager::Get().Delete(*ScreenshotPath);
    }

    if (bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Slot %d deleted"), SlotIndex);
    }

    return bSuccess;
}

bool UMotaEzSlotManager::IsSlotOccupied(int32 SlotIndex)
{
    FString ConfigPath = GetSlotConfigPath(SlotIndex);
    return FPaths::FileExists(ConfigPath);
}

FMotaEzSlotInfo UMotaEzSlotManager::GetSlotInfo(int32 SlotIndex)
{
    FMotaEzSlotInfo Info;
    LoadSlotMetadata(SlotIndex, Info);
    Info.SlotIndex = SlotIndex;
    Info.bIsOccupied = IsSlotOccupied(SlotIndex);
    return Info;
}

TArray<FMotaEzSlotInfo> UMotaEzSlotManager::GetAllSlots(int32 MaxSlots)
{
    TArray<FMotaEzSlotInfo> Slots;
    
    for (int32 i = 0; i < MaxSlots; ++i)
    {
        Slots.Add(GetSlotInfo(i));
    }

    Slots.Add(GetSlotInfo(AutoSaveSlotIndex));

    return Slots;
}

bool UMotaEzSlotManager::UpdateSlotMetadata(int32 SlotIndex, const FMotaEzSlotInfo& NewInfo)
{
    return SaveSlotMetadata(SlotIndex, NewInfo);
}

bool UMotaEzSlotManager::SaveSlotScreenshot(const UObject* WorldContextObject, int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("EzSaveGame: SaveSlotScreenshot for slot %d (full implementation pending)"), SlotIndex);
    return true;
}

FString UMotaEzSlotManager::GetSlotScreenshotPath(int32 SlotIndex)
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings) return TEXT("");

    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), Settings->SaveDirectory.Path);
    FString SlotName = (SlotIndex == AutoSaveSlotIndex) ? TEXT("AutoSave") : FString::Printf(TEXT("Slot%d"), SlotIndex);
    
    return FPaths::Combine(SaveDir, FString::Printf(TEXT("%s_Screenshot.png"), *SlotName));
}

bool UMotaEzSlotManager::AutoSave(const UObject* WorldContextObject)
{
    FString Error;
    bool bSuccess = SaveToSlot(WorldContextObject, AutoSaveSlotIndex, Error);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Auto-save completed"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Auto-save failed: %s"), *Error);
    }

    return bSuccess;
}

bool UMotaEzSlotManager::QuickSaveToSlot(const UObject* WorldContextObject, FString& OutError)
{
    int32 LastSlot = GetLastUsedSlot();
    
    if (LastSlot < 0)
    {
        LastSlot = 0;
    }

    return SaveToSlot(WorldContextObject, LastSlot, OutError);
}

bool UMotaEzSlotManager::QuickLoadFromSlot(const UObject* WorldContextObject, FString& OutError)
{
    int32 LastSlot = GetLastUsedSlot();
    
    if (LastSlot < 0 || !IsSlotOccupied(LastSlot))
    {
        OutError = TEXT("No saved slot available for Quick Load");
        return false;
    }

    return LoadFromSlot(WorldContextObject, LastSlot, OutError);
}

bool UMotaEzSlotManager::CopySlot(int32 FromSlot, int32 ToSlot)
{
    if (!IsSlotOccupied(FromSlot))
    {
        return false;
    }

    FString FromConfig = GetSlotConfigPath(FromSlot);
    FString ToConfig = GetSlotConfigPath(ToSlot);
    
    FString FromMetadata = GetSlotMetadataPath(FromSlot);
    FString ToMetadata = GetSlotMetadataPath(ToSlot);

    bool bSuccess = true;
    bSuccess &= IFileManager::Get().Copy(*ToConfig, *FromConfig) == COPY_OK;
    
    if (FPaths::FileExists(FromMetadata))
    {
        bSuccess &= IFileManager::Get().Copy(*ToMetadata, *FromMetadata) == COPY_OK;
    }

    return bSuccess;
}

int32 UMotaEzSlotManager::GetLastUsedSlot()
{
    // Try to get from global config file first
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings) return 0;

    FString ConfigPath = Settings->GetConfigFilePath();
    
    if (FPaths::FileExists(ConfigPath))
    {
        FString ConfigText;
        if (FFileHelper::LoadFileToString(ConfigText, *ConfigPath))
        {
            TMap<FString, FMotaEzValue> LoadedData;
            FString Error;
            if (UMotaEzFormatSerializer::LoadConfig(Settings->DefaultFormat, ConfigText, LoadedData, Error))
            {
                if (LoadedData.Contains(LastUsedSlotKey))
                {
                    const FMotaEzValue& Value = LoadedData[LastUsedSlotKey];
                    if (Value.Type == EMotaEzFieldType::Int)
                    {
                        int32 LastSlot = FCString::Atoi(*Value.ScalarValue);
                        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: GetLastUsedSlot - Found slot %d"), LastSlot);
                        return LastSlot;
                    }
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: GetLastUsedSlot - No previous slot found, using slot 0"));
    return 0;
}

int32 UMotaEzSlotManager::FindFirstEmptySlot(int32 MaxSlots)
{
    for (int32 i = 0; i < MaxSlots; ++i)
    {
        if (!IsSlotOccupied(i))
        {
            return i;
        }
    }
    
    return -1;
}

bool UMotaEzSlotManager::DeleteAllSaveData(int32 MaxSlots)
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to get settings for DeleteAllSaveData"));
        return false;
    }

    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), Settings->SaveDirectory.Path);
    
    if (!FPaths::DirectoryExists(SaveDir))
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Save directory does not exist: %s"), *SaveDir);
        return true; // No data to delete
    }

    bool bSuccess = true;
    int32 DeletedFiles = 0;

    // Delete all regular slots (0 to MaxSlots-1)
    for (int32 i = 0; i < MaxSlots; ++i)
    {
        if (IsSlotOccupied(i))
        {
            if (DeleteSlot(i))
            {
                DeletedFiles++;
            }
            else
            {
                bSuccess = false;
            }
        }
    }

    // Delete auto-save slot
    if (IsSlotOccupied(AutoSaveSlotIndex))
    {
        if (DeleteSlot(AutoSaveSlotIndex))
        {
            DeletedFiles++;
        }
        else
        {
            bSuccess = false;
        }
    }

    // Delete main config and schema files
    FString ConfigPath = Settings->GetConfigFilePath();
    if (FPaths::FileExists(ConfigPath))
    {
        if (IFileManager::Get().Delete(*ConfigPath))
        {
            DeletedFiles++;
            UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Deleted config file: %s"), *ConfigPath);
        }
        else
        {
            bSuccess = false;
            UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to delete config file: %s"), *ConfigPath);
        }
    }

    FString SchemaPath = Settings->GetSchemaFilePath();
    if (FPaths::FileExists(SchemaPath))
    {
        if (IFileManager::Get().Delete(*SchemaPath))
        {
            DeletedFiles++;
            UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Deleted schema file: %s"), *SchemaPath);
        }
        else
        {
            bSuccess = false;
            UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to delete schema file: %s"), *SchemaPath);
        }
    }

    if (bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Successfully deleted all save data (%d files)"), DeletedFiles);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Some files could not be deleted"));
    }

    return bSuccess;
}

bool UMotaEzSlotManager::LoadSlotMetadata(int32 SlotIndex, FMotaEzSlotInfo& OutInfo)
{
    FString MetadataPath = GetSlotMetadataPath(SlotIndex);
    
    if (!FPaths::FileExists(MetadataPath))
    {
        return false;
    }

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *MetadataPath))
    {
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return false;
    }

    OutInfo.SlotIndex = JsonObject->GetIntegerField(TEXT("SlotIndex"));
    OutInfo.SlotName = JsonObject->GetStringField(TEXT("SlotName"));
    OutInfo.PlayTime = JsonObject->GetNumberField(TEXT("PlayTime"));
    OutInfo.PlayerLevel = JsonObject->GetStringField(TEXT("PlayerLevel"));
    OutInfo.CharacterLevel = JsonObject->GetIntegerField(TEXT("CharacterLevel"));
    OutInfo.ScreenshotPath = JsonObject->GetStringField(TEXT("ScreenshotPath"));
    OutInfo.CustomData = JsonObject->GetStringField(TEXT("CustomData"));
    
    FString DateTimeString = JsonObject->GetStringField(TEXT("LastModified"));
    FDateTime::Parse(DateTimeString, OutInfo.LastModified);

    return true;
}

bool UMotaEzSlotManager::SaveSlotMetadata(int32 SlotIndex, const FMotaEzSlotInfo& Info)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    JsonObject->SetNumberField(TEXT("SlotIndex"), Info.SlotIndex);
    JsonObject->SetStringField(TEXT("SlotName"), Info.SlotName);
    JsonObject->SetStringField(TEXT("LastModified"), Info.LastModified.ToString());
    JsonObject->SetNumberField(TEXT("PlayTime"), Info.PlayTime);
    JsonObject->SetStringField(TEXT("PlayerLevel"), Info.PlayerLevel);
    JsonObject->SetNumberField(TEXT("CharacterLevel"), Info.CharacterLevel);
    JsonObject->SetStringField(TEXT("ScreenshotPath"), Info.ScreenshotPath);
    JsonObject->SetStringField(TEXT("CustomData"), Info.CustomData);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    
    if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
    {
        return false;
    }

    FString MetadataPath = GetSlotMetadataPath(SlotIndex);
    return FFileHelper::SaveStringToFile(JsonString, *MetadataPath);
}

