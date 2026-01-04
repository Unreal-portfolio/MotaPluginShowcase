// SingletonDemoActor.cpp - Implementación
#include "SingletonDemoActor.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzSlotManager.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "StatsManager.h"
#include "Kismet/GameplayStatics.h"

// SLOT ASIGNADO PARA NIVEL 1 (SINGLETON)
static const int32 SINGLETON_SLOT_INDEX = 10;

ASingletonDemoActor::ASingletonDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Crear trigger box PRIMERO como root
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	
	// Configurar trigger box
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	
	// Bind overlap event
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASingletonDemoActor::OnTriggerBeginOverlap);
	
	// Crear componente de texto 3D como hijo del trigger
	CounterText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CounterText"));
	CounterText->SetupAttachment(RootComponent);
	
	CounterText->SetHorizontalAlignment(EHTA_Center);
	CounterText->SetVerticalAlignment(EVRTA_TextCenter);
	CounterText->SetWorldSize(120.0f);
	CounterText->SetTextRenderColor(FColor::Green);
	CounterText->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	
	ActorID = TEXT("Actor");
}

void ASingletonDemoActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Cargar datos desde Slot 10 (Nivel Singleton)
	FString Error;
	UMotaEzSlotManager::LoadFromSlot(GetWorld(), SINGLETON_SLOT_INDEX, Error);
	
	// Actualizar display inicial
	UpdateDisplayText();
}

void ASingletonDemoActor::IncrementSharedCounter()
{
	// DEMOSTRACIÓN PATRÓN SINGLETON:
	// Todos los actores acceden a la MISMA instancia del subsistema
	// El contador es compartido globalmente
	
	// Obtener valor actual desde el subsistema único (Singleton)
	int32 CurrentValue = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("GlobalCounter"), 0);
	
	// Incrementar
	CurrentValue++;
	
	// Guardar de vuelta en el subsistema único
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("GlobalCounter"), CurrentValue);
	
	// Guardar en Slot 10 (Nivel Singleton)
	FString Error;
	UMotaEzSlotManager::SaveToSlot(GetWorld(), SINGLETON_SLOT_INDEX, Error);
	
	// Log para depuración
	UE_LOG(LogTemp, Log, TEXT("%s incrementó GlobalCounter a: %d"), *ActorID, CurrentValue);
	
	// Actualizar display
	UpdateDisplayText();
	
	// Buscar StatsManager para actualizar su caché
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStatsManager::StaticClass(), FoundActors);
	
	if (FoundActors.Num() > 0)
	{
		AStatsManager* StatsManager = Cast<AStatsManager>(FoundActors[0]);
		if (StatsManager)
		{
			StatsManager->RefreshStatsFromSaveSystem();
		}
	}
}

int32 ASingletonDemoActor::GetSharedCounter() const
{
	// Acceso al valor compartido desde el Singleton
	return UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("GlobalCounter"), 0);
}

void ASingletonDemoActor::UpdateDisplayText()
{
	int32 Value = GetSharedCounter();
	FString DisplayString = FString::Printf(TEXT("%s\nCounter: %d"), *ActorID, Value);
	CounterText->SetText(FText::FromString(DisplayString));
}

void ASingletonDemoActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Verificar que sea el personaje del jugador
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		// Incrementar contador compartido (PATRÓN SINGLETON)
		IncrementSharedCounter();
		
		// Feedback visual
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
				FString::Printf(TEXT("%s: Contador incrementado!"), *ActorID));
		}
	}
}

