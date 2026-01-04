// StatsManager.h - Gestiona estadísticas globales del showcase
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StatsManager.generated.h"

/**
 * Manager que demuestra el patrón SINGLETON
 * Acceso global a estadísticas del juego
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API AStatsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AStatsManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// === FUNCIONES PARA DEMOSTRAR SINGLETON ===
	
	/** Incrementa el contador global - accesible desde cualquier actor */
	UFUNCTION(BlueprintCallable, Category = "Stats|Singleton")
	void IncrementGlobalCounter();
	
	/** Obtiene el valor del contador global */
	UFUNCTION(BlueprintPure, Category = "Stats|Singleton")
	int32 GetGlobalCounter() const;
	
	/** Incrementa contador de teletransportes */
	UFUNCTION(BlueprintCallable, Category = "Stats|Singleton")
	void IncrementTeleportCount();
	
	/** Obtiene tiempo total de juego */
	UFUNCTION(BlueprintPure, Category = "Stats|Singleton")
	int32 GetTotalPlayTime() const;
	
	/** Obtiene total de datos guardados */
	UFUNCTION(BlueprintPure, Category = "Stats|Singleton")
	int32 GetTotalSavedDataCount() const;
	
	/** Actualiza todas las estadísticas desde el sistema de guardado */
	UFUNCTION(BlueprintCallable, Category = "Stats|Singleton")
	void RefreshStatsFromSaveSystem();
	
	/** Guarda todas las estadísticas actuales */
	UFUNCTION(BlueprintCallable, Category = "Stats|Singleton")
	void SaveAllStats();
	
	// === FUNCIONES PARA EL PANEL DE ESTADÍSTICAS ===
	
	/** Obtiene el formato activo de serialización como string */
	UFUNCTION(BlueprintPure, Category = "Stats|Display")
	FString GetActiveFormat() const;
	
	/** Obtiene si la encriptación está activa */
	UFUNCTION(BlueprintPure, Category = "Stats|Display")
	bool IsEncryptionEnabled() const;
	
	/** Obtiene timestamp del último guardado */
	UFUNCTION(BlueprintPure, Category = "Stats|Display")
	FString GetLastSaveTimestamp() const;
	
	/** Obtiene número de slots ocupados */
	UFUNCTION(BlueprintPure, Category = "Stats|Display")
	int32 GetOccupiedSlotsCount() const;
	
	/** Obtiene contador de teletransportes */
	UFUNCTION(BlueprintPure, Category = "Stats|Display")
	int32 GetTeleportCount() const { return CachedTeleportCount; }
	
	// === COMPONENTES ===
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;

private:
	UPROPERTY()
	int32 CachedGlobalCounter;
	
	UPROPERTY()
	int32 CachedPlayTime; // En segundos
	
	UPROPERTY()
	int32 CachedTeleportCount;
	
	UPROPERTY()
	FDateTime LastSaveTime;
	
	// Timer para actualizar el panel automáticamente
	FTimerHandle UpdateTimerHandle;
	
	void UpdateStatsDisplay();
};

