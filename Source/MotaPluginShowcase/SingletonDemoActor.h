// SingletonDemoActor.h - Demostración del patrón Singleton
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SingletonDemoActor.generated.h"

/**
 * Actor que demuestra el patrón SINGLETON
 * Muestra cómo múltiples actores acceden a la misma instancia del subsistema
 */
UCLASS()
class MOTAPLUGINSHOWCASE_API ASingletonDemoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASingletonDemoActor();

protected:
	virtual void BeginPlay() override;

public:	
	/** Incrementa el contador global compartido */
	UFUNCTION(BlueprintCallable, Category = "Singleton Demo")
	void IncrementSharedCounter();
	
	/** Obtiene el valor actual del contador global */
	UFUNCTION(BlueprintPure, Category = "Singleton Demo")
	int32 GetSharedCounter() const;
	
	/** ID único de este actor para demostración */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Singleton Demo")
	FString ActorID;
	
	/** Componente de texto 3D para mostrar el valor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTextRenderComponent* CounterText;
	
	/** Trigger box para incrementar contador */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

private:
	/** Actualiza el texto 3D con el valor actual */
	void UpdateDisplayText();
	
	/** Callback cuando algo entra en el trigger */
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};

