#include "MotaEzFormatSerializer.h"
#include "MotaEzTomlSerializer.h"
#include "MotaEzJsonSerializer.h"
#include "MotaEzYamlSerializer.h"
#include "MotaEzXmlSerializer.h"

bool UMotaEzFormatSerializer::LoadConfig(
    EMotaEzConfigFormat Format,
    const FString& InText,
    TMap<FString, FMotaEzValue>& OutData,
    FString& OutError
)
{
    switch (Format)
    {
    case EMotaEzConfigFormat::Toml:
        return UMotaEzTomlSerializer::LoadConfig(InText, OutData, OutError);

    case EMotaEzConfigFormat::Json:
        return UMotaEzJsonSerializer::LoadConfig(InText, OutData, OutError);

    case EMotaEzConfigFormat::Yaml:
        return UMotaEzYamlSerializer::LoadConfig(InText, OutData, OutError);

    case EMotaEzConfigFormat::Xml:
        return UMotaEzXmlSerializer::LoadConfig(InText, OutData, OutError);

    default:
        OutError = TEXT("Unsupported format in LoadConfig");
        return false;
    }
}

bool UMotaEzFormatSerializer::SaveConfig(
    EMotaEzConfigFormat Format,
    const TMap<FString, FMotaEzValue>& InData,
    FString& OutText,
    FString& OutError
)
{
    switch (Format)
    {
    case EMotaEzConfigFormat::Toml:
        return UMotaEzTomlSerializer::SaveConfig(InData, OutText, OutError);

    case EMotaEzConfigFormat::Json:
        return UMotaEzJsonSerializer::SaveConfig(InData, OutText, OutError);

    case EMotaEzConfigFormat::Yaml:
        return UMotaEzYamlSerializer::SaveConfig(InData, OutText, OutError);

    case EMotaEzConfigFormat::Xml:
        return UMotaEzXmlSerializer::SaveConfig(InData, OutText, OutError);

    default:
        OutError = TEXT("Unsupported format in SaveConfig");
        return false;
    }
}

bool UMotaEzFormatSerializer::LoadSchema(
    EMotaEzConfigFormat Format,
    const FString& InText,
    FMotaEzSaveSchema& OutSchema,
    FString& OutError
)
{
    switch (Format)
    {
    case EMotaEzConfigFormat::Toml:
        return UMotaEzTomlSerializer::LoadSchema(InText, OutSchema, OutError);

    case EMotaEzConfigFormat::Json:
        return UMotaEzJsonSerializer::LoadSchema(InText, OutSchema, OutError);

    case EMotaEzConfigFormat::Yaml:
        return UMotaEzYamlSerializer::LoadSchema(InText, OutSchema, OutError);

    case EMotaEzConfigFormat::Xml:
        return UMotaEzXmlSerializer::LoadSchema(InText, OutSchema, OutError);

    default:
        OutError = TEXT("Unsupported format in LoadSchema");
        return false;
    }
}

bool UMotaEzFormatSerializer::SaveSchema(
    EMotaEzConfigFormat Format,
    const FMotaEzSaveSchema& InSchema,
    FString& OutText,
    FString& OutError
)
{
    switch (Format)
    {
    case EMotaEzConfigFormat::Toml:
        return UMotaEzTomlSerializer::SaveSchema(InSchema, OutText, OutError);

    case EMotaEzConfigFormat::Json:
        return UMotaEzJsonSerializer::SaveSchema(InSchema, OutText, OutError);

    case EMotaEzConfigFormat::Yaml:
        return UMotaEzYamlSerializer::SaveSchema(InSchema, OutText, OutError);

    case EMotaEzConfigFormat::Xml:
        return UMotaEzXmlSerializer::SaveSchema(InSchema, OutText, OutError);

    default:
        OutError = TEXT("Unsupported format in SaveSchema");
        return false;
    }
}
