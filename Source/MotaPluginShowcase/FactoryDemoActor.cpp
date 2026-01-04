// FactoryDemoActor.cpp - Implementación
#include "FactoryDemoActor.h"
#include "MotaEzSchemaGenerator.h"
#include "MotaEzSaveGameLibrary.h"
#include "MotaEzSlotManager.h"
#include "SaveData.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// SLOT ASIGNADO PARA NIVEL 4 (FACTORY) - "SlotFactory"
static const int32 SLOT_FACTORY = 4;

AFactoryDemoActor::AFactoryDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	InfoText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	RootComponent = InfoText;
	
	InfoText->SetHorizontalAlignment(EHTA_Left);
	InfoText->SetVerticalAlignment(EVRTA_TextTop);
	InfoText->SetWorldSize(60.0f);
	InfoText->SetTextRenderColor(FColor::Magenta);
	
	// Crear triggers
	TriggerGenerate = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerGenerate"));
	TriggerGenerate->SetupAttachment(RootComponent);
	TriggerGenerate->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerGenerate->SetRelativeLocation(FVector(0.0f, -150.0f, 0.0f));
	TriggerGenerate->SetGenerateOverlapEvents(true);
	TriggerGenerate->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerGenerate->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerGenerate->OnComponentBeginOverlap.AddDynamic(this, &AFactoryDemoActor::OnGenerateOverlap);
	
	TriggerCreateData = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerCreateData"));
	TriggerCreateData->SetupAttachment(RootComponent);
	TriggerCreateData->SetBoxExtent(FVector(80.0f, 80.0f, 40.0f));
	TriggerCreateData->SetRelativeLocation(FVector(0.0f, 150.0f, 0.0f));
	TriggerCreateData->SetGenerateOverlapEvents(true);
	TriggerCreateData->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCreateData->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerCreateData->OnComponentBeginOverlap.AddDynamic(this, &AFactoryDemoActor::OnCreateDataOverlap);
	
	bSchemaGenerated = false;
}

void AFactoryDemoActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateDisplay();
}

bool AFactoryDemoActor::GenerateSchemaDemo()
{
	UE_LOG(LogTemp, Log, TEXT("🏭 PATRÓN FACTORY METHOD - Generando Schema..."));
	
	// El Factory Method inspecciona la clase USaveData mediante reflexión
	// y CREA automáticamente un schema con los tipos detectados
	
	bool bSuccess = UMotaEzSchemaGenerator::GenerateSchemaFromSaveGame(
		USaveData::StaticClass(),
		GeneratedSchema
	);
	
	if (bSuccess)
	{
		bSchemaGenerated = true;
		
		UE_LOG(LogTemp, Log, TEXT("✅ Schema generado automáticamente!"));
		UE_LOG(LogTemp, Log, TEXT("   Campos detectados: %d"), GeneratedSchema.Fields.Num());
		
		// Mostrar cada campo detectado por el Factory
		// Fields es un TMap<FString, FMotaEzFieldDescriptor>
		for (const auto& Pair : GeneratedSchema.Fields)
		{
			UE_LOG(LogTemp, Log, TEXT("   • %s: %s"), 
				*Pair.Key,  // Nombre del campo
				*UEnum::GetDisplayValueAsText(Pair.Value.Type).ToString());  // Tipo
		}
		
		UpdateDisplay();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Error generando schema"));
	}
	
	return bSuccess;
}

FString AFactoryDemoActor::GetSchemaInfo()
{
	if (!bSchemaGenerated)
	{
		return TEXT("Schema no generado aún");
	}
	
	FString Info = FString::Printf(
		TEXT("Campos: %d\n\n"),
		GeneratedSchema.Fields.Num()
	);
	
	// Iterar sobre el TMap
	int32 Count = 0;
	for (const auto& Pair : GeneratedSchema.Fields)
	{
		Info += FormatFieldInfo(Pair.Value) + TEXT("\n");
		Count++;
		if (Count >= 10) break;  // Limitar display
	}
	
	return Info;
}

int32 AFactoryDemoActor::GetDetectedFieldsCount() const
{
	return bSchemaGenerated ? GeneratedSchema.Fields.Num() : 0;
}

void AFactoryDemoActor::CreateDataFromSchema()
{
	if (!bSchemaGenerated)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Genera el schema primero"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("📝 Creando datos según schema generado..."));
	
	// Crear datos de ejemplo basados en los campos del schema
	// Iterar sobre TMap
	for (const auto& Pair : GeneratedSchema.Fields)
	{
		const FString& FieldName = Pair.Key;
		const FMotaEzFieldDescriptor& Field = Pair.Value;
		
		switch (Field.Type)
		{
		case EMotaEzFieldType::Int:
			UMotaEzSaveGameLibrary::EzSetInt(this, FieldName, 42);
			break;
		case EMotaEzFieldType::Float:
			UMotaEzSaveGameLibrary::EzSetFloat(this, FieldName, 3.14f);
			break;
		case EMotaEzFieldType::Bool:
			UMotaEzSaveGameLibrary::EzSetBool(this, FieldName, true);
			break;
		case EMotaEzFieldType::String:
			UMotaEzSaveGameLibrary::EzSetString(this, FieldName, TEXT("Factory Demo"));
			break;
		case EMotaEzFieldType::Vector:
			UMotaEzSaveGameLibrary::EzSetVector(this, FieldName, FVector(1, 2, 3));
			break;
		case EMotaEzFieldType::Rotator:
			UMotaEzSaveGameLibrary::EzSetRotator(this, FieldName, FRotator(0, 90, 0));
			break;
		default:
			break;
		}
	}
	
	// Guardar en Slot 4 (SlotFactory)
	FString Error;
	UMotaEzSlotManager::SaveToSlot(GetWorld(), SLOT_FACTORY, Error);
	
	UE_LOG(LogTemp, Log, TEXT("Datos creados y guardados en Slot 4 (SlotFactory)"));
	UpdateDisplay();
}

FString AFactoryDemoActor::GetFieldTypeName(int32 FieldIndex) const
{
	if (!bSchemaGenerated)
	{
		return TEXT("N/A");
	}
	
	// Iterar hasta el índice
	int32 CurrentIndex = 0;
	for (const auto& Pair : GeneratedSchema.Fields)
	{
		if (CurrentIndex == FieldIndex)
		{
			return UEnum::GetDisplayValueAsText(Pair.Value.Type).ToString();
		}
		CurrentIndex++;
	}
	
	return TEXT("N/A");
}

void AFactoryDemoActor::UpdateDisplay()
{
	FString DisplayText = TEXT("PATRÓN FACTORY METHOD\n\n");
	
	if (bSchemaGenerated)
	{
		DisplayText += FString::Printf(
			TEXT("Campos detectados: %d\n\n"),
			GeneratedSchema.Fields.Num()
		);
		
		// Mostrar primeros 5 campos
		int32 Count = 0;
		for (const auto& Pair : GeneratedSchema.Fields)
		{
			DisplayText += FormatFieldInfo(Pair.Value) + TEXT("\n");
			Count++;
			if (Count >= 5) break;
		}
		
		if (GeneratedSchema.Fields.Num() > 5)
		{
			DisplayText += FString::Printf(TEXT("... y %d más"), 
				GeneratedSchema.Fields.Num() - 5);
		}
	}
	else
	{
		DisplayText += TEXT("Presiona botón para\ngenerar schema\nautomáticamente\n\n");
		DisplayText += TEXT("El Factory Method\ninspeccionará USaveData\ny creará el schema");
	}
	
	InfoText->SetText(FText::FromString(DisplayText));
}

FString AFactoryDemoActor::FormatFieldInfo(const FMotaEzFieldDescriptor& Field)
{
	FString TypeName = UEnum::GetDisplayValueAsText(Field.Type).ToString();
	// Simplificar nombre del tipo (quitar prefijo si existe)
	TypeName.RemoveFromStart(TEXT("EMotaEzFieldType::"));
	
	return FString::Printf(TEXT("• %s: %s"), *Field.Name, *TypeName);
}

void AFactoryDemoActor::OnGenerateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		bool bSuccess = GenerateSchemaDemo();
		
		if (GEngine)
		{
			if (bSuccess)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta,
					TEXT("Schema generado automaticamente (FACTORY METHOD)"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
					TEXT("Error generando schema"));
			}
		}
	}
}

void AFactoryDemoActor::OnCreateDataOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACharacter>(OtherActor))
	{
		CreateDataFromSchema();
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
				TEXT("Datos creados segun schema"));
		}
	}
}

