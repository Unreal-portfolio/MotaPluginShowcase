// StatsManager.cpp - Implementación del gestor de estadísticas
#include "StatsManager.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzSlotManager.h"
#include "MotaEzSaveGameSettings.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AStatsManager::AStatsManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Crear componente de texto 3D
	InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	RootComponent = InfoText;
	
	InfoText->SetHorizontalAlignment(EHTA_Center);
	InfoText->SetVerticalAlignment(EVRTA_TextCenter);
	InfoText->SetWorldSize(60.0f);
	InfoText->SetTextRenderColor(FColor(100, 200, 255)); // Azul claro
	
	CachedGlobalCounter = 0;
	CachedPlayTime = 0; // En segundos
	CachedTeleportCount = 0;
	LastSaveTime = FDateTime::Now();
}

void AStatsManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Cargar estadísticas desde Slot 99 (SlotMain - estadísticas globales)
	FString Error;
	UMotaEzSlotManager::LoadFromSlot(GetWorld(), 99, Error);
	
	// Cargar PlayTime SOLO al inicio (luego se incrementa en Tick)
	CachedPlayTime = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("TotalPlayTime"), 0);
	
	// Cargar estadísticas desde el sistema
	RefreshStatsFromSaveSystem();
	
	// Timer para actualizar display cada 1 segundo
	GetWorldTimerManager().SetTimer(UpdateTimerHandle, this, &AStatsManager::RefreshStatsFromSaveSystem, 1.0f, true);
	
	UE_LOG(LogTemp, Log, TEXT("Stats Manager iniciado - PlayTime inicial: %d segundos"), CachedPlayTime);
}

void AStatsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Acumular tiempo con precisión de float
	static float AccumulatedTime = 0.0f;
	AccumulatedTime += DeltaTime;
	
	// Cuando acumulamos 1 segundo completo, incrementar el contador
	if (AccumulatedTime >= 1.0f)
	{
		CachedPlayTime += 1;
		AccumulatedTime -= 1.0f; // Mantener el resto para precisión
	}
	
	// Guardar cada minuto (cada 60 segundos)
	static float TimeSinceLastSave = 0.0f;
	TimeSinceLastSave += DeltaTime;
	
	if (TimeSinceLastSave >= 60.0f)
	{
		// Guardar stats automáticamente cada minuto
		SaveAllStats();
		TimeSinceLastSave = 0.0f;
		
		UE_LOG(LogTemp, Log, TEXT("Stats auto-guardadas (1 minuto transcurrido) - PlayTime: %d segundos"), CachedPlayTime);
	}
}

void AStatsManager::IncrementGlobalCounter()
{
	// Demostración del patrón SINGLETON
	// Acceso directo al subsistema global
	CachedGlobalCounter++;
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("GlobalCounter"), CachedGlobalCounter);
	
	UE_LOG(LogTemp, Log, TEXT("GlobalCounter incrementado a: %d"), CachedGlobalCounter);
}

int32 AStatsManager::GetGlobalCounter() const
{
	return CachedGlobalCounter;
}

void AStatsManager::IncrementTeleportCount()
{
	CachedTeleportCount++;
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("TeleportCount"), CachedTeleportCount);
}

int32 AStatsManager::GetTotalPlayTime() const
{
	return CachedPlayTime;
}

int32 AStatsManager::GetTotalSavedDataCount() const
{
	// Aquí podríamos contar las keys en el sistema
	// Por simplicidad, retornamos un valor estimado
	return 15; // Número aproximado de datos guardados en SaveData
}

void AStatsManager::RefreshStatsFromSaveSystem()
{
	// Cargar todos los valores desde EzSaveGame (excepto PlayTime que se maneja en Tick)
	CachedGlobalCounter = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("GlobalCounter"), 0);
	CachedTeleportCount = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("TeleportCount"), 0);
	
	// Actualizar display
	UpdateStatsDisplay();
}

void AStatsManager::UpdateStatsDisplay()
{
	if (!InfoText)
		return;
	
	// Calcular minutos y segundos desde el total en segundos
	int32 TotalMinutes = CachedPlayTime / 60;
	int32 Seconds = CachedPlayTime % 60;
	
	FString DisplayText = TEXT("==========================\n");
	DisplayText += TEXT("   EZSAVEGAME STATS\n");
	DisplayText += TEXT("==========================\n\n");
	DisplayText += FString::Printf(TEXT("Datos guardados: %d\n"), GetTotalSavedDataCount());
	DisplayText += FString::Printf(TEXT("Formato: %s\n"), *GetActiveFormat());
	DisplayText += FString::Printf(TEXT("Encriptacion: %s\n"), IsEncryptionEnabled() ? TEXT("SI") : TEXT("NO"));
	DisplayText += FString::Printf(TEXT("Tiempo juego: %02d:%02d\n"), TotalMinutes, Seconds);
	DisplayText += FString::Printf(TEXT("Teleports: %d\n"), CachedTeleportCount);
	DisplayText += FString::Printf(TEXT("Contador global: %d\n"), CachedGlobalCounter);
	DisplayText += TEXT("\n==========================");
	
	InfoText->SetText(FText::FromString(DisplayText));
}

void AStatsManager::SaveAllStats()
{
	// Guardar todas las estadísticas actuales en el subsistema
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("GlobalCounter"), CachedGlobalCounter);
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("TotalPlayTime"), CachedPlayTime); // Guardado como int (segundos)
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("TeleportCount"), CachedTeleportCount);
	
	// Guardar en Slot 99 (SlotMain - estadísticas globales)
	FString Error;
	bool bSuccess = UMotaEzSlotManager::SaveToSlot(GetWorld(), 99, Error);
	
	if (bSuccess)
	{
		LastSaveTime = FDateTime::Now();
		UE_LOG(LogTemp, Log, TEXT("Stats guardadas en Slot 99 (SlotMain) exitosamente"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error guardando stats en Slot 99: %s"), *Error);
	}
}

FString AStatsManager::GetActiveFormat() const
{
	const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
	if (Settings)
	{
		switch (Settings->DefaultFormat)
		{
		case EMotaEzConfigFormat::Json: return TEXT("JSON");
		case EMotaEzConfigFormat::Toml: return TEXT("TOML");
		case EMotaEzConfigFormat::Yaml: return TEXT("YAML");
		case EMotaEzConfigFormat::Xml: return TEXT("XML");
		default: return TEXT("Unknown");
		}
	}
	return TEXT("JSON");
}

bool AStatsManager::IsEncryptionEnabled() const
{
	const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
	return Settings ? Settings->bEnableEncryption : false;
}

FString AStatsManager::GetLastSaveTimestamp() const
{
	return LastSaveTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}

int32 AStatsManager::GetOccupiedSlotsCount() const
{
	int32 OccupiedCount = 0;
	TArray<FMotaEzSlotInfo> AllSlots = UMotaEzSlotManager::GetAllSlots(10);
	
	for (const FMotaEzSlotInfo& Slot : AllSlots)
	{
		if (Slot.bIsOccupied)
		{
			OccupiedCount++;
		}
	}
	
	return OccupiedCount;
}

