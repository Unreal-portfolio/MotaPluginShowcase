#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MotaEzTypes.h"
#include "MotaEzFormatSerializer.h"
#include "MotaEzConfigSubsystem.generated.h"

class UMotaEzSaveData;

/**
 * Game instance subsystem responsible for loading and saving EzSaveGame configuration.
 * 
 * Features:
 * - Reads schema and config from human-readable formats (TOML/JSON/YAML/XML)
 * - Merges with defaults into UMotaEzSaveData
 * - Exposes current save data to the game
 * - Handles automatic initialization on game start
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzConfigSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMotaEzConfigSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Gets the current save data object.
     * @return Pointer to current save data, never null after initialization
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    UMotaEzSaveData* GetCurrentSave() const { return CurrentSave; }

    /**
     * Forces reload of all data from disk.
     * Discards current in-memory changes.
     * @return true if reload was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    bool ReloadFromDisk();

    /**
     * Saves current save data to disk in human-readable format.
     * @return true if save was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    bool SaveToDisk();

private:
    UPROPERTY()
    TObjectPtr<UMotaEzSaveData> CurrentSave;

    bool LoadInternal(FString& OutError);
    bool SaveInternal(FString& OutError);

    bool LoadTextFile(const FString& FilePath, FString& OutText, FString& OutError) const;
    bool SaveTextFile(const FString& FilePath, const FString& Text, FString& OutError) const;

    void BuildSaveDataFromSchemaAndConfig(
        const FMotaEzSaveSchema& Schema,
        const TMap<FString, FMotaEzValue>& RawConfig,
        UMotaEzSaveData* OutSave
    ) const;

    void ExecuteAutoSaveOnExit();
};
