#pragma once

#include "CoreMinimal.h"
#include "MotaEzTypes.h"
#include "MotaEzTomlSerializer.generated.h"

/**
 * TOML serializer based on toml++ library.
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzTomlSerializer : public UObject
{
    GENERATED_BODY()

public:

    static bool LoadConfig(
        const FString& InText,
        TMap<FString, FMotaEzValue>& OutData,
        FString& OutError
    );

    static bool SaveConfig(
        const TMap<FString, FMotaEzValue>& InData,
        FString& OutText,
        FString& OutError
    );

    static bool LoadSchema(
        const FString& InText,
        FMotaEzSaveSchema& OutSchema,
        FString& OutError
    );

    static bool SaveSchema(
        const FMotaEzSaveSchema& InSchema,
        FString& OutText,
        FString& OutError
    );
};
