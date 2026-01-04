#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MotaEzSlotManager.generated.h"

/**
 * Metadata information for a save slot.
 * Contains details about save time, player progress, and custom data.
 */
USTRUCT(BlueprintType)
struct FMotaEzSlotInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    int32 SlotIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FString SlotName;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FDateTime LastModified;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    float PlayTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FString PlayerLevel;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    int32 CharacterLevel = 1;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    bool bIsOccupied = false;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FString ScreenshotPath;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FString CustomData;
};

/**
 * Manager for multiple save slots with metadata support.
 * Provides functionality to save, load, and manage multiple save files with associated metadata.
 * Supports special auto-save slot (index -1) and numbered slots (0-N).
 */
UCLASS(BlueprintType)
class EZSAVEGAMERUNTIME_API UMotaEzSlotManager : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Saves current data to a specific slot.
     * @param WorldContextObject - World context for subsystem access
     * @param SlotIndex - Target slot index (0-N for regular slots, -1 for auto-save)
     * @param OutError - Error message if save fails
     * @return true if save was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots", meta=(WorldContext="WorldContextObject"))
    static bool SaveToSlot(const UObject* WorldContextObject, int32 SlotIndex, FString& OutError);

    /**
     * Loads data from a specific slot.
     * @param WorldContextObject - World context for subsystem access
     * @param SlotIndex - Source slot index to load from
     * @param OutError - Error message if load fails
     * @return true if load was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots", meta=(WorldContext="WorldContextObject"))
    static bool LoadFromSlot(const UObject* WorldContextObject, int32 SlotIndex, FString& OutError);

    /**
     * Deletes a specific slot including metadata and screenshot.
     * @param SlotIndex - Slot index to delete
     * @return true if deletion was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots")
    static bool DeleteSlot(int32 SlotIndex);

    /**
     * Checks if a slot exists and contains save data.
     * @param SlotIndex - Slot index to check
     * @return true if slot is occupied, false if empty
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Slots")
    static bool IsSlotOccupied(int32 SlotIndex);


    /**
     * Retrieves metadata information for a specific slot.
     * @param SlotIndex - Slot index to query
     * @return Slot metadata information
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots")
    static FMotaEzSlotInfo GetSlotInfo(int32 SlotIndex);

    /**
     * Retrieves metadata for all available slots.
     * @param MaxSlots - Maximum number of slots to query (default: 10)
     * @return Array of slot metadata for all slots
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots")
    static TArray<FMotaEzSlotInfo> GetAllSlots(int32 MaxSlots = 10);

    /**
     * Updates the metadata for a specific slot.
     * @param SlotIndex - Slot index to update
     * @param NewInfo - New metadata information
     * @return true if update was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots")
    static bool UpdateSlotMetadata(int32 SlotIndex, const FMotaEzSlotInfo& NewInfo);

    /**
     * Captures and saves a screenshot for a slot.
     * @param WorldContextObject - World context for viewport access
     * @param SlotIndex - Target slot index
     * @return true if screenshot was saved successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots", meta=(WorldContext="WorldContextObject"))
    static bool SaveSlotScreenshot(const UObject* WorldContextObject, int32 SlotIndex);

    /**
     * Gets the file path to a slot's screenshot.
     * @param SlotIndex - Slot index to query
     * @return Absolute path to screenshot file
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Slots")
    static FString GetSlotScreenshotPath(int32 SlotIndex);

    /**
     * Saves data to special auto-save slot (index -1).
     * @param WorldContextObject - World context for subsystem access
     * @return true if auto-save was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots", meta=(WorldContext="WorldContextObject"))
    static bool AutoSave(const UObject* WorldContextObject);

    /**
     * Quick saves to the last used slot or creates a new one.
     * @param WorldContextObject - World context for subsystem access
     * @param OutError - Error message if save fails
     * @return true if save was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots", meta=(WorldContext="WorldContextObject"))
    static bool QuickSaveToSlot(const UObject* WorldContextObject, FString& OutError);

    /**
     * Quick loads from the last used slot.
     * @param WorldContextObject - World context for subsystem access
     * @param OutError - Error message if load fails
     * @return true if load was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots", meta=(WorldContext="WorldContextObject"))
    static bool QuickLoadFromSlot(const UObject* WorldContextObject, FString& OutError);

    /**
     * Copies all data from one slot to another.
     * @param FromSlot - Source slot index
     * @param ToSlot - Destination slot index
     * @return true if copy was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots")
    static bool CopySlot(int32 FromSlot, int32 ToSlot);

    /**
     * Gets the index of the most recently used slot.
     * @return Last used slot index, or 0 if none
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Slots")
    static int32 GetLastUsedSlot();

    /**
     * Finds the first available empty slot.
     * @param MaxSlots - Maximum number of slots to search (default: 10)
     * @return First empty slot index, or -1 if all slots are occupied
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Slots")
    static int32 FindFirstEmptySlot(int32 MaxSlots = 10);

    /**
     * Deletes ALL save data including all slots, metadata, screenshots, config, and schema files.
     * WARNING: This action cannot be undone!
     * @param MaxSlots - Maximum number of slots to delete (default: 10)
     * @return true if all files were deleted successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Slots")
    static bool DeleteAllSaveData(int32 MaxSlots = 10);

private:
    static FString GetSlotConfigPath(int32 SlotIndex);
    static FString GetSlotMetadataPath(int32 SlotIndex);
    static bool LoadSlotMetadata(int32 SlotIndex, FMotaEzSlotInfo& OutInfo);
    static bool SaveSlotMetadata(int32 SlotIndex, const FMotaEzSlotInfo& Info);

    static const FString LastUsedSlotKey;
    static const int32 AutoSaveSlotIndex;
};

