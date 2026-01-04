// CompositeDemoActor.cpp - Implementación
#include "CompositeDemoActor.h"
#include "MotaEzSaveGameLibrary.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ACompositeDemoActor::ACompositeDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Crear componente de texto 3D como root
	InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	RootComponent = InfoText;
	
	InfoText->SetHorizontalAlignment(EHTA_Left);
	InfoText->SetVerticalAlignment(EVRTA_TextTop);
	InfoText->SetWorldSize(60.0f);
	InfoText->SetTextRenderColor(FColor::Cyan);
	
	// === CREAR TRIGGERS PARA SLOT 0 ===
	TriggerSaveSlot0 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerSaveSlot0"));
	TriggerSaveSlot0->SetupAttachment(RootComponent);
	TriggerSaveSlot0->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	TriggerSaveSlot0->SetRelativeLocation(FVector(0.0f, -200.0f, 0.0f));
	TriggerSaveSlot0->SetGenerateOverlapEvents(true);
	TriggerSaveSlot0->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSaveSlot0->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerSaveSlot0->OnComponentBeginOverlap.AddDynamic(this, &ACompositeDemoActor::OnSaveSlot0);
	
	TriggerLoadSlot0 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerLoadSlot0"));
	TriggerLoadSlot0->SetupAttachment(RootComponent);
	TriggerLoadSlot0->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	TriggerLoadSlot0->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TriggerLoadSlot0->SetGenerateOverlapEvents(true);
	TriggerLoadSlot0->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerLoadSlot0->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerLoadSlot0->OnComponentBeginOverlap.AddDynamic(this, &ACompositeDemoActor::OnLoadSlot0);
	
	TriggerDeleteSlot0 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerDeleteSlot0"));
	TriggerDeleteSlot0->SetupAttachment(RootComponent);
	TriggerDeleteSlot0->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	TriggerDeleteSlot0->SetRelativeLocation(FVector(0.0f, 200.0f, 0.0f));
	TriggerDeleteSlot0->SetGenerateOverlapEvents(true);
	TriggerDeleteSlot0->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerDeleteSlot0->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerDeleteSlot0->OnComponentBeginOverlap.AddDynamic(this, &ACompositeDemoActor::OnDeleteSlot0);
	
	// === CREAR TRIGGERS PARA SLOT 1 ===
	TriggerSaveSlot1 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerSaveSlot1"));
	TriggerSaveSlot1->SetupAttachment(RootComponent);
	TriggerSaveSlot1->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	TriggerSaveSlot1->SetRelativeLocation(FVector(300.0f, -200.0f, 0.0f));
	TriggerSaveSlot1->SetGenerateOverlapEvents(true);
	TriggerSaveSlot1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSaveSlot1->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerSaveSlot1->OnComponentBeginOverlap.AddDynamic(this, &ACompositeDemoActor::OnSaveSlot1);
	
	TriggerLoadSlot1 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerLoadSlot1"));
	TriggerLoadSlot1->SetupAttachment(RootComponent);
	TriggerLoadSlot1->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	TriggerLoadSlot1->SetRelativeLocation(FVector(300.0f, 0.0f, 0.0f));
	TriggerLoadSlot1->SetGenerateOverlapEvents(true);
	TriggerLoadSlot1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerLoadSlot1->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerLoadSlot1->OnComponentBeginOverlap.AddDynamic(this, &ACompositeDemoActor::OnLoadSlot1);
	
	TriggerDeleteSlot1 = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerDeleteSlot1"));
	TriggerDeleteSlot1->SetupAttachment(RootComponent);
	TriggerDeleteSlot1->SetBoxExtent(FVector(50.0f, 50.0f, 30.0f));
	TriggerDeleteSlot1->SetRelativeLocation(FVector(300.0f, 200.0f, 0.0f));
	TriggerDeleteSlot1->SetGenerateOverlapEvents(true);
	TriggerDeleteSlot1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerDeleteSlot1->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerDeleteSlot1->OnComponentBeginOverlap.AddDynamic(this, &ACompositeDemoActor::OnDeleteSlot1);
}

void ACompositeDemoActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateInfoDisplay();
}

bool ACompositeDemoActor::SaveToSlotDemo(int32 SlotIndex)
{
	FString Error;
	bool bSuccess = UMotaEzSlotManager::SaveToSlot(this, SlotIndex, Error);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Guardado en Slot %d exitoso"), SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Error guardando en Slot %d: %s"), SlotIndex, *Error);
	}
	
	UpdateInfoDisplay();
	return bSuccess;
}

bool ACompositeDemoActor::LoadFromSlotDemo(int32 SlotIndex)
{
	FString Error;
	bool bSuccess = UMotaEzSlotManager::LoadFromSlot(this, SlotIndex, Error);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Carga desde Slot %d exitosa"), SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Error cargando Slot %d: %s"), SlotIndex, *Error);
	}
	
	UpdateInfoDisplay();
	return bSuccess;
}

bool ACompositeDemoActor::DeleteSlotDemo(int32 SlotIndex)
{
	bool bSuccess = UMotaEzSlotManager::DeleteSlot(SlotIndex);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("🗑️ Slot %d eliminado"), SlotIndex);
	}
	
	UpdateInfoDisplay();
	return bSuccess;
}

FMotaEzSlotInfo ACompositeDemoActor::GetSlotInfoDemo(int32 SlotIndex)
{
	return UMotaEzSlotManager::GetSlotInfo(SlotIndex);
}

TArray<FMotaEzSlotInfo> ACompositeDemoActor::GetAllSlotsDemo()
{
	return UMotaEzSlotManager::GetAllSlots(10);
}

bool ACompositeDemoActor::IsSlotOccupiedDemo(int32 SlotIndex)
{
	return UMotaEzSlotManager::IsSlotOccupied(SlotIndex);
}

void ACompositeDemoActor::CreateSampleData(int32 PlayerLevel, FString PlayerName, FVector PlayerPosition)
{
	// Crear datos de ejemplo para demostrar el guardado jerárquico
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("PlayerLevel"), PlayerLevel);
	UMotaEzSaveGameLibrary::EzSetString(this, TEXT("PlayerName"), PlayerName);
	UMotaEzSaveGameLibrary::EzSetVector(this, TEXT("PlayerPosition"), PlayerPosition);
	
	UE_LOG(LogTemp, Log, TEXT("📝 Datos de ejemplo creados: %s, Nivel %d"), *PlayerName, PlayerLevel);
}

bool ACompositeDemoActor::PerformAutoSave()
{
	bool bSuccess = UMotaEzSlotManager::AutoSave(this);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("💾 AutoSave realizado exitosamente"));
	}
	
	UpdateInfoDisplay();
	return bSuccess;
}

void ACompositeDemoActor::UpdateInfoDisplay()
{
	FString DisplayText = TEXT("PATRÓN COMPOSITE - Slots\n\n");
	
	TArray<FMotaEzSlotInfo> AllSlots = GetAllSlotsDemo();
	
	int32 OccupiedCount = 0;
	for (const FMotaEzSlotInfo& Slot : AllSlots)
	{
		if (Slot.bIsOccupied)
		{
			OccupiedCount++;
			DisplayText += FormatSlotInfo(Slot) + TEXT("\n");
		}
	}
	
	DisplayText += FString::Printf(TEXT("\nTotal Ocupados: %d / %d"), OccupiedCount, AllSlots.Num());
	
	InfoText->SetText(FText::FromString(DisplayText));
}

FString ACompositeDemoActor::FormatSlotInfo(const FMotaEzSlotInfo& SlotInfo)
{
	return FString::Printf(TEXT("Slot %d: %s (%.1fs)"),
		SlotInfo.SlotIndex,
		*SlotInfo.PlayerLevel,
		SlotInfo.PlayTime);
}

// === IMPLEMENTACIÓN DE CALLBACKS ===

void ACompositeDemoActor::OnSaveSlot0(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		// Guardar la posición actual del jugador en el subsistema (se guardará con el slot)
		FVector PlayerPosition = Character->GetActorLocation();
		UMotaEzSaveGameLibrary::EzSetVector(this, TEXT("SavedPosition"), PlayerPosition);
		
		CreateSampleData(20, TEXT("Save Slot 0"), PlayerPosition);
		
		// Guardar en slot - esto guarda todos los datos del subsistema en el slot
		SaveToSlotDemo(0);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
				FString::Printf(TEXT("Guardado en Slot 0 - Pos: %s"), *PlayerPosition.ToString()));
		}
	}
}

void ACompositeDemoActor::OnLoadSlot0(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		// Cargar desde slot - esto carga todos los datos del slot al subsistema
		bool bLoaded = LoadFromSlotDemo(0);
		
		if (bLoaded)
		{
			// Obtener la posición guardada (ahora está en el subsistema tras LoadFromSlot)
			FVector SavedPosition = UMotaEzSaveGameLibrary::EzGetVector(this, TEXT("SavedPosition"), FVector::ZeroVector);
			
			// Solo teleportar si hay una posición guardada válida (no es 0,0,0)
			if (!SavedPosition.IsNearlyZero())
			{
				FVector TeleportPosition = SavedPosition + FVector(-500, 0, 0);
				Character->SetActorLocation(TeleportPosition, false, nullptr, ETeleportType::TeleportPhysics);
				
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, 
						FString::Printf(TEXT("Cargado Slot 0 - Teleportado a: %s"), *TeleportPosition.ToString()));
				}
			}
			else
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
						TEXT("Cargado Slot 0 - Sin posicion guardada"));
				}
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
					TEXT("Slot 0 no existe o esta vacio"));
			}
		}
	}
}

void ACompositeDemoActor::OnDeleteSlot0(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		DeleteSlotDemo(0);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Slot 0 eliminado"));
		}
	}
}

void ACompositeDemoActor::OnSaveSlot1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		// Guardar la posición actual del jugador en el subsistema (se guardará con el slot)
		FVector PlayerPosition = Character->GetActorLocation();
		UMotaEzSaveGameLibrary::EzSetVector(this, TEXT("SavedPosition"), PlayerPosition);
		
		CreateSampleData(35, TEXT("Save Slot 7"), PlayerPosition);
		SaveToSlotDemo(7);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
				FString::Printf(TEXT("Guardado en Slot 7 - Pos: %s"), *PlayerPosition.ToString()));
		}
	}
}

void ACompositeDemoActor::OnLoadSlot1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		// Cargar desde slot - esto carga todos los datos del slot al subsistema
		bool bLoaded = LoadFromSlotDemo(7);
		
		if (bLoaded)
		{
			// Obtener la posición guardada (ahora está en el subsistema tras LoadFromSlot)
			FVector SavedPosition = UMotaEzSaveGameLibrary::EzGetVector(this, TEXT("SavedPosition"), FVector::ZeroVector);
			
			// Solo teleportar si hay una posición guardada válida (no es 0,0,0)
			if (!SavedPosition.IsNearlyZero())
			{
				FVector TeleportPosition = SavedPosition + FVector(-500, 0, 0);
				Character->SetActorLocation(TeleportPosition, false, nullptr, ETeleportType::TeleportPhysics);
				
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, 
						FString::Printf(TEXT("Cargado Slot 7 - Teleportado a: %s"), *TeleportPosition.ToString()));
				}
			}
			else
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
						TEXT("Cargado Slot 7 - Sin posicion guardada"));
				}
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
					TEXT("Slot 7 no existe o esta vacio"));
			}
		}
	}
}

void ACompositeDemoActor::OnDeleteSlot1(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		DeleteSlotDemo(7);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Slot 7 eliminado"));
		}
	}
}

