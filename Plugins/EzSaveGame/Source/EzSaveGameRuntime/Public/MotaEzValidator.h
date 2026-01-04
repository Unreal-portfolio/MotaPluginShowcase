#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MotaEzTypes.h"
#include "MotaEzValidator.generated.h"

/**
 * Validation result structure.
 * Contains validation status, failed field name, and error message.
 */
USTRUCT(BlueprintType)
struct FMotaEzValidationResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    bool bIsValid = true;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FString FieldName;

    UPROPERTY(BlueprintReadOnly, Category="EzSaveGame")
    FString ErrorMessage;
};

/**
 * Data validator based on schema definitions.
 * Validates values against min/max ranges, regex patterns, and custom rules.
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzValidator : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Validates a single value against its field descriptor.
     * @param FieldName - Name of the field being validated
     * @param Value - Value to validate
     * @param Descriptor - Field descriptor containing validation rules
     * @return Validation result with success status and error details if failed
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Validation")
    static FMotaEzValidationResult ValidateField(const FString& FieldName, const FMotaEzValue& Value, const FMotaEzFieldDescriptor& Descriptor);

    /**
     * Validates all data against the complete schema.
     * Returns on first validation error encountered.
     * @param Data - Map of all save data
     * @param Schema - Complete schema with validation rules
     * @return Validation result, bIsValid=true if all fields pass
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Validation")
    static FMotaEzValidationResult ValidateAllData(const TMap<FString, FMotaEzValue>& Data, const FMotaEzSaveSchema& Schema);

    /**
     * Validates a numeric value against min/max range.
     * @param Value - Numeric value to validate
     * @param MinValue - Minimum allowed value (inclusive)
     * @param MaxValue - Maximum allowed value (inclusive)
     * @param OutError - Error message if validation fails
     * @return true if value is within range, false otherwise
     */
    static bool ValidateNumberRange(float Value, float MinValue, float MaxValue, FString& OutError);

    /**
     * Validates a string against a regex pattern.
     * @param Value - String to validate
     * @param Pattern - Regex pattern to match against
     * @param OutError - Error message if validation fails
     * @return true if string matches pattern, false otherwise
     */
    static bool ValidateStringPattern(const FString& Value, const FString& Pattern, FString& OutError);

    /**
     * Validates an array by checking each element.
     * @param ArrayValues - Array of string values to validate
     * @param Descriptor - Field descriptor with validation rules
     * @param OutError - Error message if any element fails validation
     * @return true if all elements pass validation, false otherwise
     */
    static bool ValidateArray(const TArray<FString>& ArrayValues, const FMotaEzFieldDescriptor& Descriptor, FString& OutError);
};

