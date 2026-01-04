// FacadeDemoActor.cpp - Implementación
#include "FacadeDemoActor.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzConfigSubsystem.h"
#include "MotaEzSaveData.h"
#include "MotaEzSlotManager.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// SLOT ASIGNADO PARA NIVEL 3 (FACADE) - "SlotFacade"
static const int32 SLOT_FACADE = 3;

AFacadeDemoActor::AFacadeDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DisplayText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DisplayText"));
	RootComponent = DisplayText;
	
	DisplayText->SetHorizontalAlignment(EHTA_Center);
	DisplayText->SetVerticalAlignment(EVRTA_TextCenter);
	DisplayText->SetWorldSize(70.0f);
	DisplayText->SetTextRenderColor(FColor::Blue);
	
	// Crear triggers
	TriggerSetName = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerSetName"));
	TriggerSetName->SetupAttachment(RootComponent);
	TriggerSetName->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerSetName->SetRelativeLocation(FVector(0.0f, -200.0f, 0.0f));
	TriggerSetName->SetGenerateOverlapEvents(true);
	TriggerSetName->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSetName->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerSetName->OnComponentBeginOverlap.AddDynamic(this, &AFacadeDemoActor::OnSetNameOverlap);
	
	TriggerSetScore = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerSetScore"));
	TriggerSetScore->SetupAttachment(RootComponent);
	TriggerSetScore->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerSetScore->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TriggerSetScore->SetGenerateOverlapEvents(true);
	TriggerSetScore->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSetScore->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerSetScore->OnComponentBeginOverlap.AddDynamic(this, &AFacadeDemoActor::OnSetScoreOverlap);
	
	TriggerQuickSave = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerQuickSave"));
	TriggerQuickSave->SetupAttachment(RootComponent);
	TriggerQuickSave->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerQuickSave->SetRelativeLocation(FVector(0.0f, 200.0f, 0.0f));
	TriggerQuickSave->SetGenerateOverlapEvents(true);
	TriggerQuickSave->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerQuickSave->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerQuickSave->OnComponentBeginOverlap.AddDynamic(this, &AFacadeDemoActor::OnQuickSaveOverlap);
	
	ComplexOperationCount = 0;
	SimpleOperationCount = 0;
}

void AFacadeDemoActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Cargar datos desde Slot 3 (SlotFacade)
	FString Error;
	UMotaEzSlotManager::LoadFromSlot(GetWorld(), SLOT_FACADE, Error);
	
	UpdateDisplay();
}

void AFacadeDemoActor::SaveWithoutFacade(FString Key, int32 Value)
{
	// ❌ EJEMPLO COMPLEJO - Sin Facade (muchos pasos manuales)
	// Este código muestra la complejidad de acceder directamente al subsistema
	
	UE_LOG(LogTemp, Warning, TEXT("❌ Guardando SIN Facade (método complejo):"));
	UE_LOG(LogTemp, Warning, TEXT("   1. Obtener GameInstance"));
	UE_LOG(LogTemp, Warning, TEXT("   2. Obtener Subsistema"));
	UE_LOG(LogTemp, Warning, TEXT("   3. Verificar que no sea null"));
	UE_LOG(LogTemp, Warning, TEXT("   4. Obtener CurrentSave"));
	UE_LOG(LogTemp, Warning, TEXT("   5. Llamar SetInt"));
	UE_LOG(LogTemp, Warning, TEXT("   6. Llamar SaveToDisk"));
	
	// Implementación real (compleja):
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance)
	{
		UMotaEzConfigSubsystem* Subsystem = GameInstance->GetSubsystem<UMotaEzConfigSubsystem>();
		if (Subsystem)
		{
			UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
			if (SaveData)
			{
				SaveData->SetInt(Key, Value);
				bool bSuccess = Subsystem->SaveToDisk();
				
				ComplexOperationCount++;
				UE_LOG(LogTemp, Warning, TEXT("   ✓ Completado (6 pasos!)"));
			}
		}
	}
	
	UpdateDisplay();
}

void AFacadeDemoActor::SaveWithFacade(FString Key, int32 Value)
{
	// ✅ EJEMPLO SIMPLE - Con Facade (1 línea!)
	
	UE_LOG(LogTemp, Log, TEXT("✅ Guardando CON Facade (método simple):"));
	UE_LOG(LogTemp, Log, TEXT("   UMotaEzSaveGameLibrary::EzSetInt(this, Key, Value);"));
	
	// Implementación real (simple):
	UMotaEzSaveGameLibrary::EzSetInt(this, Key, Value);
	
	SimpleOperationCount++;
	UE_LOG(LogTemp, Log, TEXT("   ✓ Completado (1 línea!)"));
	
	UpdateDisplay();
}

void AFacadeDemoActor::DemoCompleteWorkflow()
{
	UE_LOG(LogTemp, Display, TEXT("=== DEMOSTRACIÓN COMPLETA DEL PATRÓN FACADE ===\n"));
	
	// Guardar varios valores usando Facade (simple y limpio)
	UMotaEzSaveGameLibrary::EzSetString(this, TEXT("PlayerName"), TEXT("Hero"));
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("PlayerScore"), 1000);
	UMotaEzSaveGameLibrary::EzSetFloat(this, TEXT("PlayerHealth"), 85.5f);
	UMotaEzSaveGameLibrary::EzSetBool(this, TEXT("HasSword"), true);
	UMotaEzSaveGameLibrary::EzSetVector(this, TEXT("SpawnPoint"), FVector(0, 0, 100));
	
	// QuickSave (1 línea!)
	bool bSuccess = UMotaEzSaveGameLibrary::QuickSave(this);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("✅ Workflow completo guardado con Facade"));
		UE_LOG(LogTemp, Display, TEXT("   - 5 valores diferentes"));
		UE_LOG(LogTemp, Display, TEXT("   - 1 QuickSave"));
		UE_LOG(LogTemp, Display, TEXT("   - Total: 6 líneas de código"));
		UE_LOG(LogTemp, Display, TEXT("   - Sin Facade serían ~30 líneas!\n"));
	}
	
	UpdateDisplay();
}

void AFacadeDemoActor::SetPlayerName(FString Name)
{
	UMotaEzSaveGameLibrary::EzSetString(this, TEXT("FacadePlayerName"), Name);
	UpdateDisplay();
}

FString AFacadeDemoActor::GetPlayerName() const
{
	return UMotaEzSaveGameLibrary::EzGetString(this, TEXT("FacadePlayerName"), TEXT("Unknown"));
}

void AFacadeDemoActor::SetPlayerScore(int32 Score)
{
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("FacadePlayerScore"), Score);
	UpdateDisplay();
}

int32 AFacadeDemoActor::GetPlayerScore() const
{
	return UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("FacadePlayerScore"), 0);
}

bool AFacadeDemoActor::QuickSaveDemo()
{
	bool bSuccess = UMotaEzSaveGameLibrary::QuickSave(this);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("💾 QuickSave realizado con Facade"));
	}
	
	UpdateDisplay();
	return bSuccess;
}

bool AFacadeDemoActor::QuickLoadDemo()
{
	bool bSuccess = UMotaEzSaveGameLibrary::QuickLoad(this);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("📂 QuickLoad realizado con Facade"));
	}
	
	UpdateDisplay();
	return bSuccess;
}

void AFacadeDemoActor::UpdateDisplay()
{
	FString Name = GetPlayerName();
	int32 Score = GetPlayerScore();
	
	FString DisplayString = FString::Printf(
		TEXT("PATRÓN FACADE\n\n")
		TEXT("Player: %s\n")
		TEXT("Score: %d\n\n")
		TEXT("Operaciones:\n")
		TEXT("Sin Facade: %d\n")
		TEXT("Con Facade: %d\n\n")
		TEXT("Reducción: ~83%%"),
		*Name, Score,
		ComplexOperationCount,
		SimpleOperationCount
	);
	
	DisplayText->SetText(FText::FromString(DisplayString));
}

void AFacadeDemoActor::OnSetNameOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		SetPlayerName(TEXT("Hero"));
		
		// Guardar en Slot 3 (SlotFacade)
		FString Error;
		UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_FACADE, Error);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
				TEXT("Nombre establecido con FACADE"));
		}
	}
}

void AFacadeDemoActor::OnSetScoreOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		// Incrementar puntuación en vez de setear a valor fijo
		int32 CurrentScore = GetPlayerScore();
		CurrentScore += 100; // Incrementa 100 cada vez
		SetPlayerScore(CurrentScore);
		
		// Guardar en Slot 3 (SlotFacade)
		FString Error;
		UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_FACADE, Error);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,
				FString::Printf(TEXT("Puntuacion incrementada a: %d"), CurrentScore));
		}
	}
}

void AFacadeDemoActor::OnQuickSaveOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		// Guardar en Slot 3 (SlotFacade) - Facade simplifica a 1 línea
		FString Error;
		bool bSuccess = UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_FACADE, Error);
		
		if (GEngine)
		{
			if (bSuccess)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
					TEXT("QuickSave (Slot 3) realizado con FACADE"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
					TEXT("Error al guardar"));
			}
		}
	}
}

