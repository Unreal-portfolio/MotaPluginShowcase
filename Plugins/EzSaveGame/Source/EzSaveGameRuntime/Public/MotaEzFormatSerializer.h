#pragma once

#include "CoreMinimal.h"
#include "MotaEzTypes.h"
#include "MotaEzFormatSerializer.generated.h"

/**
 * Human-readable file formats supported by EzSaveGame.
 */
UENUM(BlueprintType)
enum class EMotaEzConfigFormat : uint8
{
    Toml UMETA(DisplayName = "TOML"),
    Json UMETA(DisplayName = "JSON"),
    Yaml UMETA(DisplayName = "YAML"),
    Xml  UMETA(DisplayName = "XML")
};

/**
 * Format serializer router.
 * Delegates to specific serializers based on format (TOML, JSON, YAML, XML).
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzFormatSerializer : public UObject
{
    GENERATED_BODY()

public:

    static bool LoadConfig(
        EMotaEzConfigFormat Format,
        const FString& InText,
        TMap<FString, FMotaEzValue>& OutData,
        FString& OutError
    );

    static bool SaveConfig(
        EMotaEzConfigFormat Format,
        const TMap<FString, FMotaEzValue>& InData,
        FString& OutText,
        FString& OutError
    );

    static bool LoadSchema(
        EMotaEzConfigFormat Format,
        const FString& InText,
        FMotaEzSaveSchema& OutSchema,
        FString& OutError
    );

    static bool SaveSchema(
        EMotaEzConfigFormat Format,
        const FMotaEzSaveSchema& InSchema,
        FString& OutText,
        FString& OutError
    );
};

