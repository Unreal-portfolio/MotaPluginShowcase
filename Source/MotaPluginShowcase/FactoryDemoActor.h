// FactoryDemoActor.h - Demostración del patrón Factory Method
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotaEzSaveSchema.h"
#include "FactoryDemoActor.generated.h"

/**
 * Actor que demuestra el patrón FACTORY METHOD
 * Genera schemas automáticamente mediante reflexión (inspección de clases)
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API AFactoryDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFactoryDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	// === DEMOSTRACIÓN PATRÓN FACTORY METHOD ===
	
	/** Genera un schema automáticamente desde una clase SaveGame */
	UFUNCTION(BlueprintCallable, Category = "Factory Demo")
	bool GenerateSchemaDemo();
	
	/** Muestra información del schema generado */
	UFUNCTION(BlueprintCallable, Category = "Factory Demo")
	FString GetSchemaInfo();
	
	/** Obtiene número de campos detectados */
	UFUNCTION(BlueprintPure, Category = "Factory Demo")
	int32 GetDetectedFieldsCount() const;
	
	/** Crea datos de ejemplo según el schema */
	UFUNCTION(BlueprintCallable, Category = "Factory Demo")
	void CreateDataFromSchema();
	
	/** Muestra el tipo de un campo específico */
	UFUNCTION(BlueprintPure, Category = "Factory Demo")
	FString GetFieldTypeName(int32 FieldIndex) const;
	
	// === VISUALIZACIÓN ===
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerGenerate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerCreateData;

private:
	void UpdateDisplay();
	FString FormatFieldInfo(const FMotaEzFieldDescriptor& Field);
	
	UPROPERTY()
	FMotaEzSaveSchema GeneratedSchema;
	
	bool bSchemaGenerated;
	
	UFUNCTION()
	void OnGenerateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCreateDataOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

