#include "MotaEzSaveGameLibrary.h"
#include "MotaEzConfigSubsystem.h"
#include "MotaEzSaveData.h"
#include "MotaEzSaveGameSettings.h"
#include "MotaEzSchemaGenerator.h"
#include "MotaEzSlotManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "HAL/FileManager.h"

UMotaEzConfigSubsystem* UMotaEzSaveGameLibrary::GetSubsystem(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }

    return GameInstance->GetSubsystem<UMotaEzConfigSubsystem>();
}

bool UMotaEzSaveGameLibrary::QuickSave(const UObject* WorldContextObject)
{
    // QuickSave now uses the slot system - saves to the last used slot
    FString Error;
    bool bSuccess = UMotaEzSlotManager::QuickSaveToSlot(WorldContextObject, Error);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: QuickSave successful"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: QuickSave failed: %s"), *Error);
    }

    return bSuccess;
}

bool UMotaEzSaveGameLibrary::QuickLoad(const UObject* WorldContextObject)
{
    // QuickLoad now uses the slot system - loads from the last used slot
    FString Error;
    bool bSuccess = UMotaEzSlotManager::QuickLoadFromSlot(WorldContextObject, Error);
    
    if (bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: QuickLoad successful"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: QuickLoad failed: %s"), *Error);
    }

    return bSuccess;
}

int32 UMotaEzSaveGameLibrary::EzGetInt(const UObject* WorldContextObject, const FString& Key, int32 DefaultValue)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return DefaultValue;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return DefaultValue;

    return SaveData->GetInt(Key, DefaultValue);
}

float UMotaEzSaveGameLibrary::EzGetFloat(const UObject* WorldContextObject, const FString& Key, float DefaultValue)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return DefaultValue;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return DefaultValue;

    return SaveData->GetFloat(Key, DefaultValue);
}

bool UMotaEzSaveGameLibrary::EzGetBool(const UObject* WorldContextObject, const FString& Key, bool DefaultValue)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return DefaultValue;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return DefaultValue;

    return SaveData->GetBool(Key, DefaultValue);
}

FString UMotaEzSaveGameLibrary::EzGetString(const UObject* WorldContextObject, const FString& Key, const FString& DefaultValue)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return DefaultValue;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return DefaultValue;

    return SaveData->GetString(Key, DefaultValue);
}

FVector UMotaEzSaveGameLibrary::EzGetVector(const UObject* WorldContextObject, const FString& Key, FVector DefaultValue)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return DefaultValue;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return DefaultValue;

    return SaveData->GetVector(Key, DefaultValue);
}

FRotator UMotaEzSaveGameLibrary::EzGetRotator(const UObject* WorldContextObject, const FString& Key, FRotator DefaultValue)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return DefaultValue;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return DefaultValue;

    return SaveData->GetRotator(Key, DefaultValue);
}

void UMotaEzSaveGameLibrary::EzSetInt(const UObject* WorldContextObject, const FString& Key, int32 Value)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetInt(Key, Value);
}

void UMotaEzSaveGameLibrary::EzSetFloat(const UObject* WorldContextObject, const FString& Key, float Value)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetFloat(Key, Value);
}

void UMotaEzSaveGameLibrary::EzSetBool(const UObject* WorldContextObject, const FString& Key, bool Value)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetBool(Key, Value);
}

void UMotaEzSaveGameLibrary::EzSetString(const UObject* WorldContextObject, const FString& Key, const FString& Value)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetString(Key, Value);
}

void UMotaEzSaveGameLibrary::EzSetVector(const UObject* WorldContextObject, const FString& Key, FVector Value)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetVector(Key, Value);
}

void UMotaEzSaveGameLibrary::EzSetRotator(const UObject* WorldContextObject, const FString& Key, FRotator Value)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetRotator(Key, Value);
}

bool UMotaEzSaveGameLibrary::HasKey(const UObject* WorldContextObject, const FString& Key)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return false;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return false;

    FString TestValue = SaveData->GetString(Key, TEXT("__EZSAVE_NOT_FOUND__"));
    return TestValue != TEXT("__EZSAVE_NOT_FOUND__");
}

void UMotaEzSaveGameLibrary::RemoveKey(const UObject* WorldContextObject, const FString& Key)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    SaveData->SetString(Key, TEXT(""));
}

void UMotaEzSaveGameLibrary::ClearAllData(const UObject* WorldContextObject)
{
    UMotaEzConfigSubsystem* Subsystem = GetSubsystem(WorldContextObject);
    if (!Subsystem) return;

    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
    if (!SaveData) return;

    // Clear all data in memory (this will remove all keys)
    SaveData->Data.Empty();
    
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: All data in memory has been cleared"));
}


FString UMotaEzSaveGameLibrary::GetConfigFilePath()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings) return TEXT("");

    return Settings->GetConfigFilePath();
}

void UMotaEzSaveGameLibrary::OpenSaveFolder()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to get settings in OpenSaveFolder"));
        return;
    }

    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), Settings->SaveDirectory.Path);
    
    // Normalize path FIRST before checking or creating
    FPaths::NormalizeDirectoryName(SaveDir);
    SaveDir = FPaths::ConvertRelativePathToFull(SaveDir);
    
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Opening save folder: %s"), *SaveDir);

    // Create directory if it doesn't exist
    if (!FPaths::DirectoryExists(SaveDir))
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Save folder doesn't exist, creating: %s"), *SaveDir);
        if (!IFileManager::Get().MakeDirectory(*SaveDir, true))
        {
            UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to create directory: %s"), *SaveDir);
            return;
        }
    }
    
    FPlatformProcess::ExploreFolder(*SaveDir);
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: ExploreFolder called successfully"));
}

bool UMotaEzSaveGameLibrary::GenerateSchemaFromSaveGameClass(UClass* SaveGameClass, FString& OutError)
{
    if (!SaveGameClass)
    {
        OutError = TEXT("SaveGameClass is null");
        return false;
    }

    return UMotaEzSchemaGenerator::GenerateAndSaveSchema(SaveGameClass, OutError);
}

bool UMotaEzSaveGameLibrary::DeleteAllSaveData(int32 MaxSlots)
{
    return UMotaEzSlotManager::DeleteAllSaveData(MaxSlots);
}

