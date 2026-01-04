// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Teleport.generated.h"
UCLASS()
class MOTAPLUGINSHOWCASE_API ATeleport : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleport();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Componente de colisión para detectar cuando el jugador entra
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Teleport")
	UBoxComponent* TriggerBox;

	// Ubicación de destino del teletransporte (editable en el editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	FVector TeleportDestination;

	// Función que se llama cuando algo entra en el trigger
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);

};
