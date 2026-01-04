#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveData.generated.h"

/**
 * Clase de guardado personalizada para el showcase
 * Compatible con EzSaveGame - genera schema automáticamente
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API USaveData : public USaveGame
{
    GENERATED_BODY()

public:
    // === DATOS DE JUGADOR ===
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    int32 PlayerScore = 0;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    int32 PlayerLevel = 1;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    FString PlayerName = TEXT("Hero");
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    float PlayerHealth = 100.0f;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    FVector PlayerPosition = FVector::ZeroVector;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    FRotator PlayerRotation = FRotator::ZeroRotator;
    
    // === CHECKPOINT SYSTEM ===
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Checkpoint")
    FVector CheckpointPosition = FVector::ZeroVector;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Checkpoint")
    FRotator CheckpointRotation = FRotator::ZeroRotator;
    
    // === INVENTARIO ===
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Inventory")
    TArray<int32> InventoryItemIDs;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Inventory")
    TArray<int32> InventoryItemCounts;
    
    // === MISIONES ===
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Quests")
    TArray<bool> MissionCompletionStates;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Quests")
    TArray<FString> ActiveMissions;
    
    // === ESTADÍSTICAS GLOBALES ===
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    int32 GlobalCounter = 0;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    float TotalPlayTime = 0.0f;
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Stats")
    int32 TeleportCount = 0;
    
    // === VALIDACIÓN DEMO ===
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Validation")
    int32 ValidatedLevel = 1; // Min: 1, Max: 100
    
    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Validation")
    FString ValidatedPlayerName = TEXT("DefaultPlayer");
};
