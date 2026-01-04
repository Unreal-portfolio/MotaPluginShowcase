// FormatDemoActor.h - Demuestra el patrón STRATEGY con diferentes formatos
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotaEzFormatSerializer.h"
#include "FormatDemoActor.generated.h"

/**
 * Actor que demuestra el patrón STRATEGY
 * Permite cambiar el formato de serialización en runtime
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API AFormatDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFormatDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	/** Formato actualmente seleccionado */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strategy Demo")
	EMotaEzConfigFormat CurrentFormat;
	
	/** Cambia el formato de serialización */
	UFUNCTION(BlueprintCallable, Category = "Strategy Demo")
	void SetSerializationFormat(EMotaEzConfigFormat NewFormat);
	
	/** Guarda datos de prueba en el formato actual */
	UFUNCTION(BlueprintCallable, Category = "Strategy Demo")
	void SaveTestDataInCurrentFormat();
	
	/** Obtiene el nombre del formato actual */
	UFUNCTION(BlueprintPure, Category = "Strategy Demo")
	FString GetCurrentFormatName() const;
	
	/** Guarda el mismo dato en TODOS los formatos para comparación */
	UFUNCTION(BlueprintCallable, Category = "Strategy Demo")
	void SaveInAllFormats();
	
	// === COMPONENTES ===
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerJSON;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerTOML;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerYAML;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerXML;

private:
	FString FormatToString(EMotaEzConfigFormat Format) const;
	void UpdateDisplay();
	
	UFUNCTION()
	void OnJSONOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTOMLOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnYAMLOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnXMLOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

