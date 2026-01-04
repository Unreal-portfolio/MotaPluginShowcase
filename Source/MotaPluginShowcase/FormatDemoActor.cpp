// FormatDemoActor.cpp - Implementaci�n de demostraci�n de formatos
#include "FormatDemoActor.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzSaveGameSettings.h"
#include "MotaEzSlotManager.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// SLOT ASIGNADO PARA NIVEL 2 (STRATEGY) - "SlotStrategy"
static const int32 SLOT_STRATEGY = 2;

AFormatDemoActor::AFormatDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Crear texto 3D
	InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	RootComponent = InfoText;
	
	InfoText->SetHorizontalAlignment(EHTA_Center);
	InfoText->SetVerticalAlignment(EVRTA_TextCenter);
	InfoText->SetWorldSize(70.0f);
	InfoText->SetTextRenderColor(FColor::Orange);
	
	// Crear 4 triggers para cada formato
	TriggerJSON = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerJSON"));
	TriggerJSON->SetupAttachment(RootComponent);
	TriggerJSON->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerJSON->SetRelativeLocation(FVector(-300.0f, 0.0f, 0.0f));
	TriggerJSON->SetGenerateOverlapEvents(true);
	TriggerJSON->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerJSON->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerJSON->OnComponentBeginOverlap.AddDynamic(this, &AFormatDemoActor::OnJSONOverlap);
	
	TriggerTOML = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerTOML"));
	TriggerTOML->SetupAttachment(RootComponent);
	TriggerTOML->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerTOML->SetRelativeLocation(FVector(-100.0f, 0.0f, 0.0f));
	TriggerTOML->SetGenerateOverlapEvents(true);
	TriggerTOML->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerTOML->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerTOML->OnComponentBeginOverlap.AddDynamic(this, &AFormatDemoActor::OnTOMLOverlap);
	
	TriggerYAML = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerYAML"));
	TriggerYAML->SetupAttachment(RootComponent);
	TriggerYAML->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerYAML->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	TriggerYAML->SetGenerateOverlapEvents(true);
	TriggerYAML->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerYAML->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerYAML->OnComponentBeginOverlap.AddDynamic(this, &AFormatDemoActor::OnYAMLOverlap);
	
	TriggerXML = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerXML"));
	TriggerXML->SetupAttachment(RootComponent);
	TriggerXML->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerXML->SetRelativeLocation(FVector(300.0f, 0.0f, 0.0f));
	TriggerXML->SetGenerateOverlapEvents(true);
	TriggerXML->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerXML->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerXML->OnComponentBeginOverlap.AddDynamic(this, &AFormatDemoActor::OnXMLOverlap);
	
	CurrentFormat = EMotaEzConfigFormat::Json;
}

void AFormatDemoActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Cargar el formato desde configuraci�n del proyecto
	const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
	if (Settings)
	{
		CurrentFormat = Settings->DefaultFormat;
	}
	
	// Cargar datos desde Slot 11 (Nivel Strategy)
	FString Error;
	UMotaEzSlotManager::LoadFromSlot(GetWorld(), SLOT_STRATEGY, Error);
	
	UpdateDisplay();
}

void AFormatDemoActor::SetSerializationFormat(EMotaEzConfigFormat NewFormat)
{
	CurrentFormat = NewFormat;
	
	// Cambiar la configuraci�n en runtime (demostraci�n de STRATEGY)
	UMotaEzSaveGameSettings* Settings = GetMutableDefault<UMotaEzSaveGameSettings>();
	if (Settings)
	{
		Settings->DefaultFormat = NewFormat;
		UE_LOG(LogTemp, Log, TEXT("Formato cambiado a: %s"), *GetCurrentFormatName());
	}
	
	UpdateDisplay();
}

void AFormatDemoActor::SaveTestDataInCurrentFormat()
{
	// Guardar datos de prueba
	UMotaEzSaveGameLibrary::EzSetString(this, TEXT("TestData"), TEXT("Strategy Pattern Demo"));
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("FormatTestValue"), 42);
	UMotaEzSaveGameLibrary::EzSetFloat(this, TEXT("FormatTestFloat"), 3.14159f);
	
	// Guardar en Slot 11 (Nivel Strategy) con el formato actual
	FString Error;
	bool bSuccess = UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_STRATEGY, Error);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Datos guardados en Slot 11 con formato: %s"), *GetCurrentFormatName());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
				FString::Printf(TEXT("Guardado en formato: %s"), *GetCurrentFormatName()));
		}
	}
	
	UpdateDisplay();
}

FString AFormatDemoActor::GetCurrentFormatName() const
{
	return FormatToString(CurrentFormat);
}

void AFormatDemoActor::SaveInAllFormats()
{
	// Demostraci�n de que todos los formatos son intercambiables
	TArray<EMotaEzConfigFormat> AllFormats = {
		EMotaEzConfigFormat::Json,
		EMotaEzConfigFormat::Toml,
		EMotaEzConfigFormat::Yaml,
		EMotaEzConfigFormat::Xml
	};

	UE_LOG(LogTemp, Log, TEXT("=== Guardando en TODOS los formatos ==="));

	for (EMotaEzConfigFormat Format : AllFormats)
	{
		SetSerializationFormat(Format);
		SaveTestDataInCurrentFormat();
	}

	UE_LOG(LogTemp, Log, TEXT("=== Guardado completado en todos los formatos ==="));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
			TEXT("Datos guardados en: JSON, TOML, YAML y XML"));
	}
}

FString AFormatDemoActor::FormatToString(EMotaEzConfigFormat Format) const
{
	switch (Format)
	{
		case EMotaEzConfigFormat::Json: return TEXT("JSON");
		case EMotaEzConfigFormat::Toml: return TEXT("TOML");
		case EMotaEzConfigFormat::Yaml: return TEXT("YAML");
		case EMotaEzConfigFormat::Xml:  return TEXT("XML");
		default: return TEXT("Unknown");
	}
}

void AFormatDemoActor::UpdateDisplay()
{
	FString DisplayString = FString::Printf(
		TEXT("PATR�N STRATEGY\n\n")
		TEXT("Formato Actual:\n%s\n\n")
		TEXT("Entra en cada zona\npara cambiar formato"),
		*GetCurrentFormatName()
	);
	
	InfoText->SetText(FText::FromString(DisplayString));
}

void AFormatDemoActor::OnJSONOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		SetSerializationFormat(EMotaEzConfigFormat::Json);
		SaveTestDataInCurrentFormat();
	}
}

void AFormatDemoActor::OnTOMLOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		SetSerializationFormat(EMotaEzConfigFormat::Toml);
		SaveTestDataInCurrentFormat();
	}
}

void AFormatDemoActor::OnYAMLOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		SetSerializationFormat(EMotaEzConfigFormat::Yaml);
		SaveTestDataInCurrentFormat();
	}
}

void AFormatDemoActor::OnXMLOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		SetSerializationFormat(EMotaEzConfigFormat::Xml);
		SaveTestDataInCurrentFormat();
	}
}
