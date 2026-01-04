// FacadeDemoActor.h - Demostración del patrón Facade
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FacadeDemoActor.generated.h"

/**
 * Actor que demuestra el patrón FACADE
 * Compara el código complejo directo vs la API simplificada (Facade)
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API AFacadeDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFacadeDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	// === DEMOSTRACIÓN PATRÓN FACADE ===
	
	/** Ejemplo COMPLEJO - Sin usar Facade (acceso directo al subsistema) */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	void SaveWithoutFacade(FString Key, int32 Value);
	
	/** Ejemplo SIMPLE - Usando Facade (API simplificada) */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	void SaveWithFacade(FString Key, int32 Value);
	
	/** Demostración completa: guardar múltiples valores */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	void DemoCompleteWorkflow();
	
	/** Guardar nombre del jugador */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	void SetPlayerName(FString Name);
	
	/** Obtener nombre del jugador */
	UFUNCTION(BlueprintPure, Category = "Facade Demo")
	FString GetPlayerName() const;
	
	/** Guardar puntuación */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	void SetPlayerScore(int32 Score);
	
	/** Obtener puntuación */
	UFUNCTION(BlueprintPure, Category = "Facade Demo")
	int32 GetPlayerScore() const;
	
	/** QuickSave usando Facade */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	bool QuickSaveDemo();
	
	/** QuickLoad usando Facade */
	UFUNCTION(BlueprintCallable, Category = "Facade Demo")
	bool QuickLoadDemo();
	
	// === VISUALIZACIÓN ===
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* DisplayText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerSetName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerSetScore;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerQuickSave;

private:
	void UpdateDisplay();
	int32 ComplexOperationCount;
	int32 SimpleOperationCount;
	
	UFUNCTION()
	void OnSetNameOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSetScoreOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnQuickSaveOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

