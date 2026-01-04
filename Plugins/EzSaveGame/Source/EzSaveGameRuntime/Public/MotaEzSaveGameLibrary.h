/*
 * EzSaveGame Plugin - usage requires attribution to Jose Antonio Mota Lucas per the LICENSE file.
 */
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MotaEzTypes.h"
#include "MotaEzSaveGameLibrary.generated.h"

/**
 * Blueprint function library for simplified EzSaveGame usage.
 * Provides convenient wrapper functions that eliminate the need to manually obtain the subsystem.
 * All functions automatically handle subsystem retrieval and null checking.
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzSaveGameLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * Saves all current data to disk immediately.
     * @param WorldContextObject - World context for subsystem access
     * @return true if save was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Quick", meta=(WorldContext="WorldContextObject"))
    static bool QuickSave(const UObject* WorldContextObject);

    /**
     * Reloads all data from disk immediately.
     * @param WorldContextObject - World context for subsystem access
     * @return true if load was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Quick", meta=(WorldContext="WorldContextObject"))
    static bool QuickLoad(const UObject* WorldContextObject);

    /**
     * Retrieves an integer value from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param DefaultValue - Value to return if key doesn't exist
     * @return The stored integer value or default if not found
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Get", meta=(WorldContext="WorldContextObject"))
    static int32 EzGetInt(const UObject* WorldContextObject, const FString& Key, int32 DefaultValue = 0);

    /**
     * Retrieves a float value from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param DefaultValue - Value to return if key doesn't exist
     * @return The stored float value or default if not found
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Get", meta=(WorldContext="WorldContextObject"))
    static float EzGetFloat(const UObject* WorldContextObject, const FString& Key, float DefaultValue = 0.0f);

    /**
     * Retrieves a boolean value from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param DefaultValue - Value to return if key doesn't exist
     * @return The stored boolean value or default if not found
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Get", meta=(WorldContext="WorldContextObject"))
    static bool EzGetBool(const UObject* WorldContextObject, const FString& Key, bool DefaultValue = false);

    /**
     * Retrieves a string value from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param DefaultValue - Value to return if key doesn't exist
     * @return The stored string value or default if not found
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Get", meta=(WorldContext="WorldContextObject"))
    static FString EzGetString(const UObject* WorldContextObject, const FString& Key, const FString& DefaultValue = TEXT(""));

    /**
     * Retrieves a vector value from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param DefaultValue - Value to return if key doesn't exist
     * @return The stored vector value or default if not found
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Get", meta=(WorldContext="WorldContextObject"))
    static FVector EzGetVector(const UObject* WorldContextObject, const FString& Key, FVector DefaultValue = FVector::ZeroVector);

    /**
     * Retrieves a rotator value from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param DefaultValue - Value to return if key doesn't exist
     * @return The stored rotator value or default if not found
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Get", meta=(WorldContext="WorldContextObject"))
    static FRotator EzGetRotator(const UObject* WorldContextObject, const FString& Key, FRotator DefaultValue = FRotator::ZeroRotator);

    /**
     * Stores an integer value in save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param Value - Integer value to store
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Set", meta=(WorldContext="WorldContextObject"))
    static void EzSetInt(const UObject* WorldContextObject, const FString& Key, int32 Value);

    /**
     * Stores a float value in save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param Value - Float value to store
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Set", meta=(WorldContext="WorldContextObject"))
    static void EzSetFloat(const UObject* WorldContextObject, const FString& Key, float Value);

    /**
     * Stores a boolean value in save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param Value - Boolean value to store
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Set", meta=(WorldContext="WorldContextObject"))
    static void EzSetBool(const UObject* WorldContextObject, const FString& Key, bool Value);

    /**
     * Stores a string value in save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param Value - String value to store
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Set", meta=(WorldContext="WorldContextObject"))
    static void EzSetString(const UObject* WorldContextObject, const FString& Key, const FString& Value);

    /**
     * Stores a vector value in save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param Value - Vector value to store
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Set", meta=(WorldContext="WorldContextObject"))
    static void EzSetVector(const UObject* WorldContextObject, const FString& Key, FVector Value);

    /**
     * Stores a rotator value in save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier for the value
     * @param Value - Rotator value to store
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Set", meta=(WorldContext="WorldContextObject"))
    static void EzSetRotator(const UObject* WorldContextObject, const FString& Key, FRotator Value);

    /**
     * Checks if a key exists in the save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier to check
     * @return true if the key exists, false otherwise
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Utils", meta=(WorldContext="WorldContextObject"))
    static bool HasKey(const UObject* WorldContextObject, const FString& Key);

    /**
     * Removes a key from save data.
     * @param WorldContextObject - World context for subsystem access
     * @param Key - Unique identifier to remove
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Utils", meta=(WorldContext="WorldContextObject"))
    static void RemoveKey(const UObject* WorldContextObject, const FString& Key);

    /**
     * Clears all save data in memory.
     * Use this before adding new data to ensure old data doesn't accumulate.
     * NOTE: This only clears data in memory. Call QuickSave or SaveToSlot afterwards to persist the changes.
     * @param WorldContextObject - World context for subsystem access
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Utils", meta=(WorldContext="WorldContextObject"))
    static void ClearAllData(const UObject* WorldContextObject);


    /**
     * Gets the full file path of the current config file.
     * @return Absolute path to the config file
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Utils")
    static FString GetConfigFilePath();

    /**
     * Opens the save folder in the system file explorer.
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Utils")
    static void OpenSaveFolder();

    /**
     * Generates a schema automatically from a USaveGame class and saves it to disk.
     * Inspects all UPROPERTY fields marked with SaveGame specifier.
     * @param SaveGameClass - USaveGame-derived class to inspect
     * @param OutError - Error message if generation fails
     * @return true if schema was generated and saved successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Schema", meta=(DevelopmentOnly))
    static bool GenerateSchemaFromSaveGameClass(UClass* SaveGameClass, FString& OutError);

    /**
     * Deletes ALL save data including all slots, metadata, screenshots, config, and schema files.
     * WARNING: This action cannot be undone! Use with caution.
     * @param MaxSlots - Maximum number of slots to delete (default: 10)
     * @return true if all files were deleted successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Utils")
    static bool DeleteAllSaveData(int32 MaxSlots = 10);

private:
    static class UMotaEzConfigSubsystem* GetSubsystem(const UObject* WorldContextObject);
};
