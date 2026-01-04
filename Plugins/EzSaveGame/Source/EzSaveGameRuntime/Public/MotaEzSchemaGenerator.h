#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MotaEzTypes.h"
#include "MotaEzSchemaGenerator.generated.h"

/**
 * Automatic schema generator for USaveGame classes using reflection.
 * Inspects UPROPERTY fields marked with SaveGame specifier and generates
 * schema definitions with proper types and default values.
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzSchemaGenerator : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Generates a schema from a USaveGame-derived class.
     * Uses reflection to inspect all UPROPERTY fields with SaveGame specifier.
     * @param SaveGameClass - USaveGame-derived class to inspect
     * @param OutSchema - Generated schema structure
     * @return true if schema was generated successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Schema")
    static bool GenerateSchemaFromSaveGame(UClass* SaveGameClass, FMotaEzSaveSchema& OutSchema);

    /**
     * Generates a schema and saves it to disk in the configured format.
     * Combines GenerateSchemaFromSaveGame with automatic serialization and file saving.
     * @param SaveGameClass - USaveGame-derived class to inspect
     * @param OutError - Error message if generation or save fails
     * @return true if schema was generated and saved successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Schema")
    static bool GenerateAndSaveSchema(UClass* SaveGameClass, FString& OutError);

    /**
     * Inspects a property and creates a field descriptor.
     * @param Property - Property to inspect
     * @param OutDescriptor - Generated field descriptor
     * @return true if property is valid for schema, false otherwise
     */
    static bool InspectProperty(FProperty* Property, FMotaEzFieldDescriptor& OutDescriptor);

    /**
     * Detects the EMotaEzFieldType from an FProperty.
     * @param Property - Property to inspect
     * @param bIsArray - Output parameter indicating if property is an array
     * @return Detected field type, or None if unsupported
     */
    static EMotaEzFieldType DetectFieldType(FProperty* Property, bool& bIsArray);

    /**
     * Gets the default value from a property as string.
     * @param Property - Property to read
     * @param PropertyData - Pointer to property data
     * @return String representation of default value
     */
    static FString GetDefaultValueFromProperty(FProperty* Property, void* PropertyData);

private:
    static void GetDefaultArrayValues(FProperty* Property, void* PropertyData, TArray<FString>& OutValues);
};

