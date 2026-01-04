#pragma once

#include "CoreMinimal.h"
#include "MotaEzTypes.generated.h"

/**
 * Generic data type supported by EzSaveGame plugin.
 * Used to define field types in schema and runtime data validation.
 */
UENUM(BlueprintType)
enum class EMotaEzFieldType : uint8
{
    None    UMETA(DisplayName = "None"),
    Int     UMETA(DisplayName = "Int"),
    Float   UMETA(DisplayName = "Float"),
    Bool    UMETA(DisplayName = "Bool"),
    String  UMETA(DisplayName = "String"),
    Vector  UMETA(DisplayName = "Vector"),
    Rotator UMETA(DisplayName = "Rotator")
};

/**
 * Generic value container.
 * Can hold either a scalar value or an array of values of a specific type.
 * All values are stored internally as strings for easy serialization across formats.
 */
USTRUCT(BlueprintType)
struct FMotaEzValue
{
    GENERATED_BODY();

    UPROPERTY()
    EMotaEzFieldType Type = EMotaEzFieldType::None;

    UPROPERTY()
    bool bIsArray = false;

    UPROPERTY()
    FString ScalarValue;

    UPROPERTY()
    TArray<FString> ArrayValues;

    bool IsArray() const { return bIsArray; }
};

/**
 * Field descriptor for schema definition.
 * Defines the structure, type, default values, and validation rules for a save data field.
 */
USTRUCT(BlueprintType)
struct FMotaEzFieldDescriptor
{
    GENERATED_BODY();

    UPROPERTY()
    FString Name;

    UPROPERTY()
    EMotaEzFieldType Type = EMotaEzFieldType::None;

    UPROPERTY()
    bool bIsArray = false;

    UPROPERTY()
    FString DefaultValue;

    UPROPERTY()
    TArray<FString> DefaultArray;

    UPROPERTY()
    bool bEnableValidation = false;

    UPROPERTY()
    float MinValue = 0.0f;

    UPROPERTY()
    float MaxValue = 0.0f;

    UPROPERTY()
    FString RegexPattern;

    UPROPERTY()
    FString ValidationErrorMessage;
};

/**
 * Complete save schema definition.
 * Contains all field descriptors that define the structure of save data.
 */
USTRUCT(BlueprintType)
struct FMotaEzSaveSchema
{
    GENERATED_BODY();

    UPROPERTY()
    TMap<FString, FMotaEzFieldDescriptor> Fields;
};
