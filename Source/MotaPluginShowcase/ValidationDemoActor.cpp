// ValidationDemoActor.cpp - Implementaci�n de validaci�n
#include "ValidationDemoActor.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzSlotManager.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// SLOT ASIGNADO PARA NIVEL 5 (TEMPLATE METHOD) - "SlotTemplate"
static const int32 SLOT_TEMPLATE = 5;

AValidationDemoActor::AValidationDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Valores por defecto de validaci�n
	MinLevel = 1;
	MaxLevel = 100;
	MinNameLength = 3;
	MaxNameLength = 20;
	ValidCount = 0;
	InvalidCount = 0;
	
	// Crear texto 3D
	InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	RootComponent = InfoText;
	
	InfoText->SetHorizontalAlignment(EHTA_Center);
	InfoText->SetVerticalAlignment(EVRTA_TextCenter);
	InfoText->SetWorldSize(70.0f);
	InfoText->SetTextRenderColor(FColor::Yellow);
	
	// Trigger para valor v�lido
	TriggerValidValue = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerValidValue"));
	TriggerValidValue->SetupAttachment(RootComponent);
	TriggerValidValue->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerValidValue->SetRelativeLocation(FVector(0.0f, -150.0f, 0.0f));
	TriggerValidValue->SetGenerateOverlapEvents(true);
	TriggerValidValue->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerValidValue->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerValidValue->OnComponentBeginOverlap.AddDynamic(this, &AValidationDemoActor::OnValidOverlap);
	
	// Trigger para valor inv�lido
	TriggerInvalidValue = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerInvalidValue"));
	TriggerInvalidValue->SetupAttachment(RootComponent);
	TriggerInvalidValue->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerInvalidValue->SetRelativeLocation(FVector(0.0f, 150.0f, 0.0f));
	TriggerInvalidValue->SetGenerateOverlapEvents(true);
	TriggerInvalidValue->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerInvalidValue->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerInvalidValue->OnComponentBeginOverlap.AddDynamic(this, &AValidationDemoActor::OnInvalidOverlap);
}

void AValidationDemoActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Cargar datos desde Slot 5 (SlotTemplate)
	FString Error;
	UMotaEzSlotManager::LoadFromSlot(GetWorld(), SLOT_TEMPLATE, Error);
	
	UpdateDisplay();
}

bool AValidationDemoActor::SavePlayerLevelWithValidation(int32 Level, FString& OutErrorMessage)
{
	// TEMPLATE METHOD - El algoritmo de validaci�n est� definido
	// pero los pasos espec�ficos son personalizables
	
	// Paso 1: Validar tipo (siempre se ejecuta)
	if (!ValidateLevel(Level, OutErrorMessage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Validaci�n fall�: %s"), *OutErrorMessage);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
				FString::Printf(TEXT("? %s"), *OutErrorMessage));
		}
		
		InvalidCount++;
		UpdateDisplay();
		return false;
	}
	
	// Paso 2: Si pasa validación, guardar
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("ValidatedLevel"), Level);
	
	// Paso 3: Persistir en Slot 5 (SlotTemplate)
	FString SaveError;
	bool bSuccess = UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_TEMPLATE, SaveError);
	
	if (bSuccess)
	{
		OutErrorMessage = TEXT("Nivel guardado exitosamente");
		UE_LOG(LogTemp, Log, TEXT("Nivel %d guardado tras validación en Slot 5"), Level);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
				FString::Printf(TEXT("? Nivel %d guardado"), Level));
		}
		
		ValidCount++;
		UpdateDisplay();
	}
	
	return bSuccess;
}

bool AValidationDemoActor::SavePlayerNameWithValidation(const FString& PlayerName, FString& OutErrorMessage)
{
	// TEMPLATE METHOD - Mismo flujo, diferentes reglas
	
	// Paso 1: Validar
	if (!ValidateName(PlayerName, OutErrorMessage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Validaci�n de nombre fall�: %s"), *OutErrorMessage);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
				FString::Printf(TEXT("? %s"), *OutErrorMessage));
		}
		
		InvalidCount++;
		UpdateDisplay();
		return false;
	}
	
	// Paso 2: Guardar
	UMotaEzSaveGameLibrary::EzSetString(this, TEXT("ValidatedPlayerName"), PlayerName);
	
	// Paso 3: Persistir en Slot 5 (SlotTemplate)
	FString SaveError;
	bool bSuccess = UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_TEMPLATE, SaveError);
	
	if (bSuccess)
	{
		OutErrorMessage = TEXT("Nombre guardado exitosamente");
		UE_LOG(LogTemp, Log, TEXT("Nombre '%s' guardado tras validación en Slot 5"), *PlayerName);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
				FString::Printf(TEXT("? Nombre '%s' guardado"), *PlayerName));
		}
		
		ValidCount++;
		UpdateDisplay();
	}
	
	return bSuccess;
}

bool AValidationDemoActor::ValidateLevel(int32 Level, FString& OutErrorMessage) const
{
	// Validaci�n de rango num�rico (extensible)
	if (Level < MinLevel)
	{
		OutErrorMessage = FString::Printf(TEXT("Nivel %d < m�nimo (%d)"), Level, MinLevel);
		return false;
	}
	
	if (Level > MaxLevel)
	{
		OutErrorMessage = FString::Printf(TEXT("Nivel %d > m�ximo (%d)"), Level, MaxLevel);
		return false;
	}
	
	OutErrorMessage = TEXT("Validaci�n exitosa");
	return true;
}

bool AValidationDemoActor::ValidateName(const FString& Name, FString& OutErrorMessage) const
{
	// Validaci�n de cadena (extensible)
	if (Name.IsEmpty())
	{
		OutErrorMessage = TEXT("El nombre no puede estar vac�o");
		return false;
	}
	
	if (Name.Len() < MinNameLength)
	{
		OutErrorMessage = FString::Printf(TEXT("Nombre muy corto (m�n: %d)"), MinNameLength);
		return false;
	}
	
	if (Name.Len() > MaxNameLength)
	{
		OutErrorMessage = FString::Printf(TEXT("Nombre muy largo (m�x: %d)"), MaxNameLength);
		return false;
	}
	
	OutErrorMessage = TEXT("Validaci�n exitosa");
	return true;
}

void AValidationDemoActor::UpdateDisplay()
{
	FString DisplayString = FString::Printf(
		TEXT("PATR�N TEMPLATE METHOD\n\n")
		TEXT("Flujo de Validaci�n:\n")
		TEXT("1. Validar Tipo\n")
		TEXT("2. Validar Rango\n")
		TEXT("3. Guardar\n\n")
		TEXT("V�lidos: %d\n")
		TEXT("Inv�lidos: %d"),
		ValidCount,
		InvalidCount
	);
	
	InfoText->SetText(FText::FromString(DisplayString));
}

void AValidationDemoActor::OnValidOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		// Probar con valor v�lido (75 est� en rango 1-100)
		FString Error;
		SavePlayerLevelWithValidation(75, Error);
	}
}

void AValidationDemoActor::OnInvalidOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		// Probar con valor inválido (150 está fuera de rango 1-100)
		FString Error;
		SavePlayerLevelWithValidation(150, Error);
	}
}
