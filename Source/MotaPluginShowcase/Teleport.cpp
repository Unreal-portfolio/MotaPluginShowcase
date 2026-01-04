// Fill out your copyright notice in the Description page of Project Settings.


#include "Teleport.h"
#include "GameFramework/Character.h"
#include "MotaEzSaveGameLibrary.h"
#include "StatsManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATeleport::ATeleport()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Crear el componente de colisión
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	// Configurar el tamaño del trigger box
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	// Configurar el trigger box para generar eventos de overlap
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	// Vincular la función de overlap
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATeleport::OnOverlapBegin);

	// Inicializar la ubicación de destino
	TeleportDestination = FVector(0.0f, 0.0f, 0.0f);
}

// Called when the game starts or when spawned
void ATeleport::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeleport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleport::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	// Verificar que el actor que entra sea un personaje (jugador)
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	
	if (Character)
	{
		// Teletransportar al personaje a la ubicación de destino
		Character->SetActorLocation(TeleportDestination, false, nullptr, ETeleportType::TeleportPhysics);
		
		// Buscar el StatsManager y actualizar el contador global
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStatsManager::StaticClass(), FoundActors);
		
		if (FoundActors.Num() > 0)
		{
			AStatsManager* StatsManager = Cast<AStatsManager>(FoundActors[0]);
			if (StatsManager)
			{
				StatsManager->IncrementTeleportCount();
			}
		}
		
		// Log para debugging
		UE_LOG(LogTemp, Log, TEXT("Teletransporte realizado a: %s"), *TeleportDestination.ToString());
	}
}
