#include "MotaEzSchemaGenerator.h"
#include "MotaEzFormatSerializer.h"
#include "MotaEzSaveGameSettings.h"
#include "GameFramework/SaveGame.h"
#include "Misc/FileHelper.h"
#include "UObject/TextProperty.h"

bool UMotaEzSchemaGenerator::GenerateSchemaFromSaveGame(UClass* SaveGameClass, FMotaEzSaveSchema& OutSchema)
{
    OutSchema.Fields.Reset();

    if (!SaveGameClass)
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: SaveGameClass is null"));
        return false;
    }

    if (!SaveGameClass->IsChildOf(USaveGame::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: %s does not inherit from USaveGame"), *SaveGameClass->GetName());
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("EzSaveGame: Generating schema from %s"), *SaveGameClass->GetName());

    UObject* CDO = SaveGameClass->GetDefaultObject();

    for (TFieldIterator<FProperty> PropIt(SaveGameClass); PropIt; ++PropIt)
    {
        FProperty* Property = *PropIt;

        if (!Property->HasAnyPropertyFlags(CPF_SaveGame))
        {
            continue;
        }

        FMotaEzFieldDescriptor Descriptor;
        if (InspectProperty(Property, Descriptor))
        {
            void* PropertyData = Property->ContainerPtrToValuePtr<void>(CDO);
            Descriptor.DefaultValue = GetDefaultValueFromProperty(Property, PropertyData);

            if (Descriptor.bIsArray)
            {
                GetDefaultArrayValues(Property, PropertyData, Descriptor.DefaultArray);
            }

            OutSchema.Fields.Add(Descriptor.Name, Descriptor);
            
            UE_LOG(LogTemp, Log, TEXT("  %s (%s) - Default: %s"), 
                *Descriptor.Name, 
                Descriptor.bIsArray ? TEXT("Array") : TEXT("Scalar"),
                *Descriptor.DefaultValue);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("EzSaveGame: Schema generated with %d fields"), OutSchema.Fields.Num());
    return OutSchema.Fields.Num() > 0;
}

bool UMotaEzSchemaGenerator::GenerateAndSaveSchema(UClass* SaveGameClass, FString& OutError)
{
    FMotaEzSaveSchema Schema;
    if (!GenerateSchemaFromSaveGame(SaveGameClass, Schema))
    {
        OutError = TEXT("Failed to generate schema from class");
        return false;
    }

    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        OutError = TEXT("Failed to get settings");
        return false;
    }

    FString SchemaText;
    if (!UMotaEzFormatSerializer::SaveSchema(Settings->DefaultFormat, Schema, SchemaText, OutError))
    {
        return false;
    }

    FString FilePath = Settings->GetSchemaFilePath();
    if (!FFileHelper::SaveStringToFile(SchemaText, *FilePath))
    {
        OutError = FString::Printf(TEXT("Failed to write file: %s"), *FilePath);
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Schema saved to: %s"), *FilePath);
    return true;
}

bool UMotaEzSchemaGenerator::InspectProperty(FProperty* Property, FMotaEzFieldDescriptor& OutDescriptor)
{
    if (!Property)
    {
        return false;
    }

    OutDescriptor.Name = Property->GetName();

    bool bIsArray = false;
    OutDescriptor.Type = DetectFieldType(Property, bIsArray);
    OutDescriptor.bIsArray = bIsArray;

    if (OutDescriptor.Type == EMotaEzFieldType::None)
    {
        UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Could not detect type of %s"), *OutDescriptor.Name);
        return false;
    }

    return true;
}

EMotaEzFieldType UMotaEzSchemaGenerator::DetectFieldType(FProperty* Property, bool& bIsArray)
{
    bIsArray = false;

    if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
    {
        bIsArray = true;
        Property = ArrayProp->Inner;
    }

    if (CastField<FIntProperty>(Property) || CastField<FInt64Property>(Property))
    {
        return EMotaEzFieldType::Int;
    }

    if (CastField<FFloatProperty>(Property) || CastField<FDoubleProperty>(Property))
    {
        return EMotaEzFieldType::Float;
    }

    if (CastField<FBoolProperty>(Property))
    {
        return EMotaEzFieldType::Bool;
    }

    if (CastField<FStrProperty>(Property) || CastField<FNameProperty>(Property) || CastField<FTextProperty>(Property))
    {
        return EMotaEzFieldType::String;
    }

    if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
    {
        if (StructProp->Struct == TBaseStructure<FVector>::Get())
        {
            return EMotaEzFieldType::Vector;
        }
        
        if (StructProp->Struct == TBaseStructure<FRotator>::Get())
        {
            return EMotaEzFieldType::Rotator;
        }
    }

    return EMotaEzFieldType::None;
}

FString UMotaEzSchemaGenerator::GetDefaultValueFromProperty(FProperty* Property, void* PropertyData)
{
    if (!Property || !PropertyData)
    {
        return TEXT("");
    }

    if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
    {
        return TEXT("");
    }

    if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
    {
        int32 Value = IntProp->GetPropertyValue(PropertyData);
        return FString::FromInt(Value);
    }

    if (FInt64Property* Int64Prop = CastField<FInt64Property>(Property))
    {
        int64 Value = Int64Prop->GetPropertyValue(PropertyData);
        return FString::Printf(TEXT("%lld"), Value);
    }

    if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
    {
        float Value = FloatProp->GetPropertyValue(PropertyData);
        return FString::SanitizeFloat(Value);
    }

    if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
    {
        double Value = DoubleProp->GetPropertyValue(PropertyData);
        return FString::Printf(TEXT("%f"), Value);
    }

    if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
    {
        bool Value = BoolProp->GetPropertyValue(PropertyData);
        return Value ? TEXT("true") : TEXT("false");
    }

    if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
    {
        return StrProp->GetPropertyValue(PropertyData);
    }

    if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
    {
        FName Value = NameProp->GetPropertyValue(PropertyData);
        return Value.ToString();
    }

    if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
    {
        FText Value = TextProp->GetPropertyValue(PropertyData);
        return Value.ToString();
    }

    if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
    {
        if (StructProp->Struct == TBaseStructure<FVector>::Get())
        {
            FVector* VectorPtr = static_cast<FVector*>(PropertyData);
            return FString::Printf(TEXT("%f,%f,%f"), VectorPtr->X, VectorPtr->Y, VectorPtr->Z);
        }
        
        if (StructProp->Struct == TBaseStructure<FRotator>::Get())
        {
            FRotator* RotatorPtr = static_cast<FRotator*>(PropertyData);
            return FString::Printf(TEXT("%f,%f,%f"), RotatorPtr->Pitch, RotatorPtr->Yaw, RotatorPtr->Roll);
        }
    }

    return TEXT("");
}

void UMotaEzSchemaGenerator::GetDefaultArrayValues(FProperty* Property, void* PropertyData, TArray<FString>& OutValues)
{
    OutValues.Reset();

    FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property);
    if (!ArrayProp || !PropertyData)
    {
        return;
    }

    FScriptArrayHelper ArrayHelper(ArrayProp, PropertyData);
    int32 ArraySize = ArrayHelper.Num();

    for (int32 i = 0; i < ArraySize; ++i)
    {
        void* ElementData = ArrayHelper.GetRawPtr(i);
        FString ElementValue = GetDefaultValueFromProperty(ArrayProp->Inner, ElementData);
        OutValues.Add(ElementValue);
    }
}

