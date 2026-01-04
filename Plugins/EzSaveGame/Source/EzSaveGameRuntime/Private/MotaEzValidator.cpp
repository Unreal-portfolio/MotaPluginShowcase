// MotaEzValidator.cpp

#include "MotaEzValidator.h"
#include "Internationalization/Regex.h"

FMotaEzValidationResult UMotaEzValidator::ValidateField(const FString& FieldName, const FMotaEzValue& Value, const FMotaEzFieldDescriptor& Descriptor)
{
    FMotaEzValidationResult Result;
    Result.bIsValid = true;
    Result.FieldName = FieldName;
    
    if (!Descriptor.bEnableValidation)
    {
        return Result;
    }
    
    if (Value.bIsArray)
    {
        FString Error;
        if (!ValidateArray(Value.ArrayValues, Descriptor, Error))
        {
            Result.bIsValid = false;
            Result.ErrorMessage = Error;
        }
        return Result;
    }
    
    switch (Descriptor.Type)
    {
        case EMotaEzFieldType::Int:
        case EMotaEzFieldType::Float:
        {
            float NumValue = FCString::Atof(*Value.ScalarValue);
            FString Error;
            if (!ValidateNumberRange(NumValue, Descriptor.MinValue, Descriptor.MaxValue, Error))
            {
                Result.bIsValid = false;
                Result.ErrorMessage = Descriptor.ValidationErrorMessage.IsEmpty() ? Error : Descriptor.ValidationErrorMessage;
            }
            break;
        }

        case EMotaEzFieldType::String:
        {
            if (!Descriptor.RegexPattern.IsEmpty())
            {
                FString Error;
                if (!ValidateStringPattern(Value.ScalarValue, Descriptor.RegexPattern, Error))
                {
                    Result.bIsValid = false;
                    Result.ErrorMessage = Descriptor.ValidationErrorMessage.IsEmpty() ? Error : Descriptor.ValidationErrorMessage;
                }
            }
            break;
        }

        default:
            break;
    }

    return Result;
}

FMotaEzValidationResult UMotaEzValidator::ValidateAllData(const TMap<FString, FMotaEzValue>& Data, const FMotaEzSaveSchema& Schema)
{
    FMotaEzValidationResult FinalResult;
    FinalResult.bIsValid = true;

    for (const auto& Pair : Schema.Fields)
    {
        const FString& FieldName = Pair.Key;
        const FMotaEzFieldDescriptor& Descriptor = Pair.Value;
        
        if (!Data.Contains(FieldName))
        {
            continue;
        }

        const FMotaEzValue& Value = Data[FieldName];
        FMotaEzValidationResult FieldResult = ValidateField(FieldName, Value, Descriptor);

        if (!FieldResult.bIsValid)
        {
            return FieldResult;
        }
    }

    return FinalResult;
}

bool UMotaEzValidator::ValidateNumberRange(float Value, float MinValue, float MaxValue, FString& OutError)
{
    if (MinValue == 0.0f && MaxValue == 0.0f)
    {
        return true;
    }

    if (Value < MinValue || Value > MaxValue)
    {
        OutError = FString::Printf(TEXT("El valor %.2f está fuera del rango permitido [%.2f, %.2f]"), Value, MinValue, MaxValue);
        return false;
    }

    return true;
}

bool UMotaEzValidator::ValidateStringPattern(const FString& Value, const FString& Pattern, FString& OutError)
{
    if (Pattern.IsEmpty())
    {
        return true;
    }

    FRegexPattern RegexPattern(Pattern);
    FRegexMatcher Matcher(RegexPattern, Value);

    if (!Matcher.FindNext())
    {
        OutError = FString::Printf(TEXT("El valor '%s' no coincide con el patrón requerido: %s"), *Value, *Pattern);
        return false;
    }

    return true;
}

bool UMotaEzValidator::ValidateArray(const TArray<FString>& ArrayValues, const FMotaEzFieldDescriptor& Descriptor, FString& OutError)
{
    for (int32 i = 0; i < ArrayValues.Num(); ++i)
    {
        const FString& ElementValue = ArrayValues[i];
        
        switch (Descriptor.Type)
        {
            case EMotaEzFieldType::Int:
            case EMotaEzFieldType::Float:
            {
                float NumValue = FCString::Atof(*ElementValue);
                if (!ValidateNumberRange(NumValue, Descriptor.MinValue, Descriptor.MaxValue, OutError))
                {
                    OutError = FString::Printf(TEXT("Elemento [%d]: %s"), i, *OutError);
                    return false;
                }
                break;
            }

            case EMotaEzFieldType::String:
            {
                if (!Descriptor.RegexPattern.IsEmpty())
                {
                    if (!ValidateStringPattern(ElementValue, Descriptor.RegexPattern, OutError))
                    {
                        OutError = FString::Printf(TEXT("Elemento [%d]: %s"), i, *OutError);
                        return false;
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    return true;
}

