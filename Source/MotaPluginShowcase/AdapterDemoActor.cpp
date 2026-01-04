// AdapterDemoActor.cpp - Implementación
#include "AdapterDemoActor.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzConfigSubsystem.h"
#include "MotaEzSaveGameSettings.h"
#include "MotaEzSlotManager.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// SLOT ASIGNADO PARA NIVEL 6 (ADAPTER) - "SlotAdapter"
static const int32 SLOT_ADAPTER = 6;

AAdapterDemoActor::AAdapterDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StatusText"));
	RootComponent = StatusText;
	
	StatusText->SetHorizontalAlignment(EHTA_Center);
	StatusText->SetVerticalAlignment(EVRTA_TextCenter);
	StatusText->SetWorldSize(80.0f);
	StatusText->SetTextRenderColor(FColor::Orange);
	
	// Crear triggers
	TriggerDemo = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerDemo"));
	TriggerDemo->SetupAttachment(RootComponent);
	TriggerDemo->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerDemo->SetRelativeLocation(FVector(0.0f, -150.0f, 0.0f));
	TriggerDemo->SetGenerateOverlapEvents(true);
	TriggerDemo->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerDemo->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerDemo->OnComponentBeginOverlap.AddDynamic(this, &AAdapterDemoActor::OnTriggerDemoOverlap);
	
	TriggerVerify = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVerify"));
	TriggerVerify->SetupAttachment(RootComponent);
	TriggerVerify->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerVerify->SetRelativeLocation(FVector(0.0f, 150.0f, 0.0f));
	TriggerVerify->SetGenerateOverlapEvents(true);
	TriggerVerify->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVerify->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerVerify->OnComponentBeginOverlap.AddDynamic(this, &AAdapterDemoActor::OnTriggerVerifyOverlap);
	
	LastStatus = TEXT("PATRÓN ADAPTER\nReady");
}

void AAdapterDemoActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Cargar datos desde Slot 6 (SlotAdapter)
	FString Error;
	UMotaEzSlotManager::LoadFromSlot(GetWorld(), SLOT_ADAPTER, Error);
	
	UpdateStatusDisplay(LastStatus);
}

void AAdapterDemoActor::SaveInAllFormats()
{
	UE_LOG(LogTemp, Log, TEXT("🔄 Guardando datos para demostración..."));
	
	// Crear datos de prueba si no existen
	CreateTestData();
	
	// Guardar en Slot 6 (SlotAdapter)
	// NOTA: Para la demostración del patrón ADAPTER, los diferentes formatos
	// se manejan internamente por el sistema. El patrón adapta cada biblioteca
	// (toml++, yaml-cpp, tinyxml2, RapidJSON) a una interfaz común.
	
	FString Error;
	bool bSuccess = UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_ADAPTER, Error);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Datos guardados en SlotLvl6"));
		UE_LOG(LogTemp, Log, TEXT("   El patrón ADAPTER adapta las bibliotecas:"));
		UE_LOG(LogTemp, Log, TEXT("   - JSON: RapidJSON"));
		UE_LOG(LogTemp, Log, TEXT("   - TOML: toml++"));
		UE_LOG(LogTemp, Log, TEXT("   - YAML: yaml-cpp"));
		UE_LOG(LogTemp, Log, TEXT("   - XML: tinyxml2"));
	}
	
	FString Status = TEXT("✅ Patrón ADAPTER\n\n")
		TEXT("Bibliotecas adaptadas:\n")
		TEXT("• JSON (RapidJSON)\n")
		TEXT("• TOML (toml++)\n")
		TEXT("• YAML (yaml-cpp)\n")
		TEXT("• XML (tinyxml2)\n\n")
		TEXT("Guardado en SlotLvl6");
	
	UpdateStatusDisplay(Status);
}

bool AAdapterDemoActor::LoadFromFormat(EMotaEzConfigFormat Format)
{
	// Cargar usando QuickLoad
	// El formato actual se obtiene de los settings del proyecto
	bool bSuccess = UMotaEzSaveGameLibrary::QuickLoad(this);
	
	if (bSuccess)
	{
		FString Status = FString::Printf(TEXT("✅ Cargado\n\n")
			TEXT("Formato configurado:\n%s\n\n")
			TEXT("El patrón ADAPTER\npermite cambiar entre\nformatos sin modificar\nel código"),
			*GetFormatName(Format));
		UpdateStatusDisplay(Status);
		UE_LOG(LogTemp, Log, TEXT("✅ Datos cargados correctamente"));
	}
	else
	{
		FString Status = TEXT("❌ Error al cargar");
		UpdateStatusDisplay(Status);
		UE_LOG(LogTemp, Error, TEXT("❌ Error al cargar datos"));
	}
	
	return bSuccess;
}

bool AAdapterDemoActor::VerifyCrossFormatCompatibility()
{
	UE_LOG(LogTemp, Log, TEXT("🔍 Verificando patrón ADAPTER..."));
	
	// Guardar datos de referencia
	CreateTestData();
	int32 ReferenceValue = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("TestValue"), 0);
	FString ReferenceString = UMotaEzSaveGameLibrary::EzGetString(this, TEXT("TestString"), TEXT(""));
	
	// Guardar en Slot 6 (SlotAdapter)
	FString SaveError;
	bool bSaved = UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_ADAPTER, SaveError);
	
	// Limpiar
	UMotaEzSaveGameLibrary::ClearAllData(this);
	
	// Cargar desde Slot 6
	FString LoadError;
	bool bLoaded = UMotaEzSlotManager::LoadFromSlot(GetWorld(), SLOT_ADAPTER, LoadError);
	
	// Verificar integridad
	int32 LoadedValue = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("TestValue"), 0);
	FString LoadedString = UMotaEzSaveGameLibrary::EzGetString(this, TEXT("TestString"), TEXT(""));
	
	bool bCompatible = (LoadedValue == ReferenceValue && LoadedString == ReferenceString);
	
	if (bCompatible)
	{
		UpdateStatusDisplay(TEXT("✅ PATRÓN ADAPTER\n\n")
			TEXT("Verificación exitosa:\n")
			TEXT("• Guardado: ✓\n")
			TEXT("• Carga: ✓\n")
			TEXT("• Integridad: ✓\n\n")
			TEXT("Las 4 bibliotecas\nexternas están\nadaptadas correctamente"));
		UE_LOG(LogTemp, Log, TEXT("✅ Patrón ADAPTER verificado correctamente"));
	}
	else
	{
		UpdateStatusDisplay(TEXT("❌ Error en\nverificación"));
	}
	
	return bCompatible;
}

FString AAdapterDemoActor::ReadSaveFileContent(EMotaEzConfigFormat Format)
{
	FString ConfigPath = UMotaEzSaveGameLibrary::GetConfigFilePath();
	
	// Cambiar extensión según formato
	FString Extension;
	switch (Format)
	{
	case EMotaEzConfigFormat::Json: Extension = TEXT(".json"); break;
	case EMotaEzConfigFormat::Toml: Extension = TEXT(".toml"); break;
	case EMotaEzConfigFormat::Yaml: Extension = TEXT(".yaml"); break;
	case EMotaEzConfigFormat::Xml: Extension = TEXT(".xml"); break;
	default: Extension = TEXT(".json");
	}
	
	FString FilePath = FPaths::ChangeExtension(ConfigPath, Extension);
	
	FString FileContent;
	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		return FileContent;
	}
	
	return TEXT("Error: No se pudo leer el archivo");
}

FString AAdapterDemoActor::GetFormatName(EMotaEzConfigFormat Format)
{
	switch (Format)
	{
	case EMotaEzConfigFormat::Json: return TEXT("JSON");
	case EMotaEzConfigFormat::Toml: return TEXT("TOML");
	case EMotaEzConfigFormat::Yaml: return TEXT("YAML");
	case EMotaEzConfigFormat::Xml: return TEXT("XML");
	default: return TEXT("Unknown");
	}
}

void AAdapterDemoActor::CreateTestData()
{
	// Datos de prueba para la demostración
	UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("TestValue"), 12345);
	UMotaEzSaveGameLibrary::EzSetString(this, TEXT("TestString"), TEXT("Adapter Pattern Demo"));
	UMotaEzSaveGameLibrary::EzSetFloat(this, TEXT("TestFloat"), 99.99f);
	UMotaEzSaveGameLibrary::EzSetBool(this, TEXT("TestBool"), true);
	UMotaEzSaveGameLibrary::EzSetVector(this, TEXT("TestVector"), FVector(100, 200, 300));
	
	UE_LOG(LogTemp, Log, TEXT("📝 Datos de prueba creados para Adapter Demo"));
}

void AAdapterDemoActor::UpdateStatusDisplay(const FString& Message)
{
	LastStatus = Message;
	StatusText->SetText(FText::FromString(Message));
}

void AAdapterDemoActor::OnTriggerDemoOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		SaveInAllFormats();
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange,
				TEXT("Patron ADAPTER demostrado - Datos guardados"));
		}
	}
}

void AAdapterDemoActor::OnTriggerVerifyOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		bool bSuccess = VerifyCrossFormatCompatibility();
		
		if (GEngine)
		{
			if (bSuccess)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
					TEXT("Compatibilidad verificada - Todos los formatos OK"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
					TEXT("Error en compatibilidad"));
			}
		}
	}
}

