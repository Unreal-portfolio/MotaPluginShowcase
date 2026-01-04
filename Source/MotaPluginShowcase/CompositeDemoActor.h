// CompositeDemoActor.h - Demostración del patrón Composite
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotaEzSlotManager.h"
#include "CompositeDemoActor.generated.h"

/**
 * Actor que demuestra el patrón COMPOSITE
 * Gestiona múltiples slots como una jerarquía (slots + metadata + screenshots)
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API ACompositeDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACompositeDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	// === OPERACIONES CON SLOTS (Patrón Composite) ===
	
	/** Guarda datos en un slot específico */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	bool SaveToSlotDemo(int32 SlotIndex);
	
	/** Carga datos desde un slot específico */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	bool LoadFromSlotDemo(int32 SlotIndex);
	
	/** Elimina un slot específico */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	bool DeleteSlotDemo(int32 SlotIndex);
	
	/** Obtiene información de un slot */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	FMotaEzSlotInfo GetSlotInfoDemo(int32 SlotIndex);
	
	/** Obtiene todos los slots disponibles */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	TArray<FMotaEzSlotInfo> GetAllSlotsDemo();
	
	/** Verifica si un slot está ocupado */
	UFUNCTION(BlueprintPure, Category = "Composite Demo")
	bool IsSlotOccupiedDemo(int32 SlotIndex);
	
	/** Crea datos de ejemplo para guardar */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	void CreateSampleData(int32 PlayerLevel, FString PlayerName, FVector PlayerPosition);
	
	/** AutoSave (slot especial -1) */
	UFUNCTION(BlueprintCallable, Category = "Composite Demo")
	bool PerformAutoSave();
	
	// === VISUALIZACIÓN ===
	
	/** Componente de texto para mostrar información */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;
	
	// === TRIGGERS PARA SLOT 0 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerSaveSlot0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerLoadSlot0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerDeleteSlot0;
	
	// === TRIGGERS PARA SLOT 1 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerSaveSlot1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerLoadSlot1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerDeleteSlot1;

private:
	void UpdateInfoDisplay();
	FString FormatSlotInfo(const FMotaEzSlotInfo& SlotInfo);
	
	// Callbacks de triggers
	UFUNCTION()
	void OnSaveSlot0(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnLoadSlot0(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnDeleteSlot0(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSaveSlot1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnLoadSlot1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnDeleteSlot1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

