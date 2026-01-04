#pragma once

#include "CoreMinimal.h"
#include "MotaEzTypes.h"
#include "MotaEzXmlSerializer.generated.h"

/**
 * XML serializer based on tinyxml2 library.
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzXmlSerializer : public UObject
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

