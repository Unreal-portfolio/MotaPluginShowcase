// ValidationDemoActor.h - Demuestra el patrón TEMPLATE METHOD con validación
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ValidationDemoActor.generated.h"

/**
 * Actor que demuestra el patrón TEMPLATE METHOD
 * Sistema de validación con reglas personalizables
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API AValidationDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AValidationDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	/** Intenta guardar un nivel de jugador con validación */
	UFUNCTION(BlueprintCallable, Category = "Validation Demo")
	bool SavePlayerLevelWithValidation(int32 Level, FString& OutErrorMessage);
	
	/** Intenta guardar un nombre de jugador con validación */
	UFUNCTION(BlueprintCallable, Category = "Validation Demo")
	bool SavePlayerNameWithValidation(const FString& PlayerName, FString& OutErrorMessage);
	
	/** Valida un nivel manualmente (demostración del patrón) */
	UFUNCTION(BlueprintPure, Category = "Validation Demo")
	bool ValidateLevel(int32 Level, FString& OutErrorMessage) const;
	
	/** Valida un nombre manualmente (demostración del patrón) */
	UFUNCTION(BlueprintPure, Category = "Validation Demo")
	bool ValidateName(const FString& Name, FString& OutErrorMessage) const;
	
	/** Rangos de validación configurables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation Demo")
	int32 MinLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation Demo")
	int32 MaxLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation Demo")
	int32 MinNameLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation Demo")
	int32 MaxNameLength;
	
	// === COMPONENTES ===
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* InfoText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerValidValue;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerInvalidValue;

private:
	void UpdateDisplay();
	int32 ValidCount;
	int32 InvalidCount;
	
	UFUNCTION()
	void OnValidOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnInvalidOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

