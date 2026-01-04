// AdapterDemoActor.h - Demostración del patrón Adapter
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotaEzTypes.h"
#include "AdapterDemoActor.generated.h"

/**
 * Actor que demuestra el patrón ADAPTER
 * Muestra cómo el mismo dato se adapta a diferentes formatos (JSON, TOML, YAML, XML)
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API AAdapterDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAdapterDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	// === DEMOSTRACIÓN DEL PATRÓN ADAPTER ===
	
	/** Guarda datos en TODOS los formatos disponibles */
	UFUNCTION(BlueprintCallable, Category = "Adapter Demo")
	void SaveInAllFormats();
	
	/** Carga datos desde un formato específico */
	UFUNCTION(BlueprintCallable, Category = "Adapter Demo")
	bool LoadFromFormat(EMotaEzConfigFormat Format);
	
	/** Verifica que los datos sean idénticos entre formatos */
	UFUNCTION(BlueprintCallable, Category = "Adapter Demo")
	bool VerifyCrossFormatCompatibility();
	
	/** Lee el contenido de un archivo de guardado */
	UFUNCTION(BlueprintCallable, Category = "Adapter Demo")
	FString ReadSaveFileContent(EMotaEzConfigFormat Format);
	
	/** Obtiene el nombre del formato como string */
	UFUNCTION(BlueprintPure, Category = "Adapter Demo")
	static FString GetFormatName(EMotaEzConfigFormat Format);
	
	/** Crea datos de prueba para la demostración */
	UFUNCTION(BlueprintCallable, Category = "Adapter Demo")
	void CreateTestData();
	
	// === VISUALIZACIÓN ===
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* StatusText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerDemo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerVerify;

private:
	void UpdateStatusDisplay(const FString& Message);
	FString LastStatus;
	
	UFUNCTION()
	void OnTriggerDemoOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTriggerVerifyOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

