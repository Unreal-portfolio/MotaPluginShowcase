#include "MotaEzJsonSerializer.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    EMotaEzFieldType ParseTypeFromString(const FString& TypeStr)
    {
        if (TypeStr.Equals(TEXT("Int"), ESearchCase::IgnoreCase))     return EMotaEzFieldType::Int;
        if (TypeStr.Equals(TEXT("Float"), ESearchCase::IgnoreCase))   return EMotaEzFieldType::Float;
        if (TypeStr.Equals(TEXT("Bool"), ESearchCase::IgnoreCase))    return EMotaEzFieldType::Bool;
        if (TypeStr.Equals(TEXT("String"), ESearchCase::IgnoreCase))  return EMotaEzFieldType::String;
        if (TypeStr.Equals(TEXT("Vector"), ESearchCase::IgnoreCase))  return EMotaEzFieldType::Vector;
        if (TypeStr.Equals(TEXT("Rotator"), ESearchCase::IgnoreCase)) return EMotaEzFieldType::Rotator;
        return EMotaEzFieldType::None;
    }

    FString TypeToString(EMotaEzFieldType Type)
    {
        switch (Type)
        {
        case EMotaEzFieldType::Int:     return TEXT("Int");
        case EMotaEzFieldType::Float:   return TEXT("Float");
        case EMotaEzFieldType::Bool:    return TEXT("Bool");
        case EMotaEzFieldType::String:  return TEXT("String");
        case EMotaEzFieldType::Vector:  return TEXT("Vector");
        case EMotaEzFieldType::Rotator: return TEXT("Rotator");
        default:                        return TEXT("None");
        }
    }
}

bool UMotaEzJsonSerializer::LoadConfig(
    const FString& InText,
    TMap<FString, FMotaEzValue>& OutData,
    FString& OutError
)
{
    OutError.Reset();
    OutData.Reset();

    TSharedPtr<FJsonObject> RootObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InText);

    if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
    {
        OutError = TEXT("Error parseando JSON de config");
        return false;
    }

    for (const auto& Pair : RootObj->Values)
    {
        const FString& Key = Pair.Key;
        const TSharedPtr<FJsonValue>& Val = Pair.Value;

        FMotaEzValue OutVal;
        OutVal.bIsArray = Val->Type == EJson::Array;

        if (Val->Type == EJson::Array)
        {
            const TArray<TSharedPtr<FJsonValue>>& Arr = Val->AsArray();

            bool bAllInt = true;
            bool bAllFloat = true;
            bool bAllBool = true;
            bool bAllString = true;

            for (const auto& Elem : Arr)
            {
                switch (Elem->Type)
                {
                case EJson::Number:
                    break;
                case EJson::String:
                    bAllInt = bAllFloat = bAllBool = false;
                    break;
                case EJson::Boolean:
                    bAllInt = bAllFloat = bAllString = false;
                    break;
                default:
                    bAllInt = bAllFloat = bAllBool = false;
                    break;
                }
            }

            if (bAllInt)
            {
                OutVal.Type = EMotaEzFieldType::Int;
                for (const auto& Elem : Arr)
                {
                    int32 Num = (int32)Elem->AsNumber();
                    OutVal.ArrayValues.Add(FString::FromInt(Num));
                }
            }
            else if (bAllFloat)
            {
                OutVal.Type = EMotaEzFieldType::Float;
                for (const auto& Elem : Arr)
                {
                    OutVal.ArrayValues.Add(FString::SanitizeFloat((float)Elem->AsNumber()));
                }
            }
            else if (bAllBool)
            {
                OutVal.Type = EMotaEzFieldType::Bool;
                for (const auto& Elem : Arr)
                {
                    OutVal.ArrayValues.Add(Elem->AsBool() ? TEXT("true") : TEXT("false"));
                }
            }
            else
            {
                OutVal.Type = EMotaEzFieldType::String;
                for (const auto& Elem : Arr)
                {
                    OutVal.ArrayValues.Add(Elem->AsString());
                }
            }
        }
        else
        {
            OutVal.bIsArray = false;

            switch (Val->Type)
            {
            case EJson::Number:
                OutVal.Type = EMotaEzFieldType::Float;
                OutVal.ScalarValue = FString::SanitizeFloat((float)Val->AsNumber());
                break;
            case EJson::String:
                OutVal.Type = EMotaEzFieldType::String;
                OutVal.ScalarValue = Val->AsString();
                
                // Auto-detect Vector and Rotator types based on key suffix
                if (Key.EndsWith(TEXT("_Pos")) || Key.EndsWith(TEXT("Position")) || 
                    Key.Contains(TEXT("Location")) || Key.Contains(TEXT("Vector")))
                {
                    TArray<FString> Parts;
                    OutVal.ScalarValue.ParseIntoArray(Parts, TEXT(","), true);
                    if (Parts.Num() == 3)
                    {
                        OutVal.Type = EMotaEzFieldType::Vector;
                    }
                }
                else if (Key.EndsWith(TEXT("_Rot")) || Key.EndsWith(TEXT("Rotation")) || 
                         Key.Contains(TEXT("Rotator")))
                {
                    TArray<FString> Parts;
                    OutVal.ScalarValue.ParseIntoArray(Parts, TEXT(","), true);
                    if (Parts.Num() == 3)
                    {
                        OutVal.Type = EMotaEzFieldType::Rotator;
                    }
                }
                break;
            case EJson::Boolean:
                OutVal.Type = EMotaEzFieldType::Bool;
                OutVal.ScalarValue = Val->AsBool() ? TEXT("true") : TEXT("false");
                break;
            default:
                OutVal.Type = EMotaEzFieldType::String;
                OutVal.ScalarValue = TEXT("");
                break;
            }
        }

        OutData.Add(Key, OutVal);
    }

    return true;
}

bool UMotaEzJsonSerializer::SaveConfig(
    const TMap<FString, FMotaEzValue>& InData,
    FString& OutText,
    FString& OutError
)
{
    OutError.Reset();
    OutText.Reset();

    TSharedRef<FJsonObject> RootObj = MakeShared<FJsonObject>();

    for (const auto& Pair : InData)
    {
        const FString& Key = Pair.Key;
        const FMotaEzValue& Val = Pair.Value;

        if (!Val.bIsArray)
        {
            switch (Val.Type)
            {
            case EMotaEzFieldType::Int:
                RootObj->SetNumberField(Key, FCString::Atoi(*Val.ScalarValue));
                break;
            case EMotaEzFieldType::Float:
                RootObj->SetNumberField(Key, FCString::Atof(*Val.ScalarValue));
                break;
            case EMotaEzFieldType::Bool:
                RootObj->SetBoolField(
                    Key,
                    Val.ScalarValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
                    Val.ScalarValue.Equals(TEXT("1"), ESearchCase::IgnoreCase)
                );
                break;
            default:
                RootObj->SetStringField(Key, Val.ScalarValue);
                break;
            }
        }
        else
        {
            TArray<TSharedPtr<FJsonValue>> Arr;

            switch (Val.Type)
            {
            case EMotaEzFieldType::Int:
                for (const FString& S : Val.ArrayValues)
                {
                    Arr.Add(MakeShared<FJsonValueNumber>((double)FCString::Atoi(*S)));
                }
                break;
            case EMotaEzFieldType::Float:
                for (const FString& S : Val.ArrayValues)
                {
                    Arr.Add(MakeShared<FJsonValueNumber>((double)FCString::Atof(*S)));
                }
                break;
            case EMotaEzFieldType::Bool:
                for (const FString& S : Val.ArrayValues)
                {
                    bool b =
                        S.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
                        S.Equals(TEXT("1"), ESearchCase::IgnoreCase);
                    Arr.Add(MakeShared<FJsonValueBoolean>(b));
                }
                break;
            default:
                for (const FString& S : Val.ArrayValues)
                {
                    Arr.Add(MakeShared<FJsonValueString>(S));
                }
                break;
            }

            RootObj->SetArrayField(Key, Arr);
        }
    }

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutText);
    if (!FJsonSerializer::Serialize(RootObj, Writer))
    {
        OutError = TEXT("Error serializando JSON de config");
        return false;
    }

    return true;
}

bool UMotaEzJsonSerializer::LoadSchema(
    const FString& InText,
    FMotaEzSaveSchema& OutSchema,
    FString& OutError
)
{
    OutError.Reset();
    OutSchema.Fields.Reset();

    TSharedPtr<FJsonObject> RootObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InText);

    if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
    {
        OutError = TEXT("Error parseando JSON de schema");
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* FieldsArray = nullptr;
    if (!RootObj->TryGetArrayField(TEXT("Fields"), FieldsArray) || !FieldsArray)
    {
        // schema vacío no es error
        return true;
    }

    for (const TSharedPtr<FJsonValue>& Elem : *FieldsArray)
    {
        TSharedPtr<FJsonObject> FieldObj = Elem->AsObject();
        if (!FieldObj.IsValid())
        {
            continue;
        }

        FMotaEzFieldDescriptor Desc;

        FieldObj->TryGetStringField(TEXT("Name"), Desc.Name);
        if (Desc.Name.IsEmpty())
        {
            continue;
        }

        FString TypeStr;
        if (FieldObj->TryGetStringField(TEXT("Type"), TypeStr))
        {
            Desc.Type = ParseTypeFromString(TypeStr);
        }

        FieldObj->TryGetBoolField(TEXT("IsArray"), Desc.bIsArray);
        FieldObj->TryGetStringField(TEXT("DefaultValue"), Desc.DefaultValue);

        const TArray<TSharedPtr<FJsonValue>>* DefArr = nullptr;
        if (FieldObj->TryGetArrayField(TEXT("DefaultArray"), DefArr) && DefArr)
        {
            for (const TSharedPtr<FJsonValue>& V : *DefArr)
            {
                Desc.DefaultArray.Add(V->AsString());
            }
        }

        OutSchema.Fields.Add(Desc.Name, Desc);
    }

    return true;
}

bool UMotaEzJsonSerializer::SaveSchema(
    const FMotaEzSaveSchema& InSchema,
    FString& OutText,
    FString& OutError
)
{
    OutError.Reset();
    OutText.Reset();

    TSharedRef<FJsonObject> RootObj = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> FieldsArray;

    for (const auto& Pair : InSchema.Fields)
    {
        const FMotaEzFieldDescriptor& Desc = Pair.Value;

        TSharedRef<FJsonObject> FieldObj = MakeShared<FJsonObject>();
        FieldObj->SetStringField(TEXT("Name"), Desc.Name);
        FieldObj->SetStringField(TEXT("Type"), TypeToString(Desc.Type));
        FieldObj->SetBoolField(TEXT("IsArray"), Desc.bIsArray);
        FieldObj->SetStringField(TEXT("DefaultValue"), Desc.DefaultValue);

        TArray<TSharedPtr<FJsonValue>> DefArr;
        for (const FString& S : Desc.DefaultArray)
        {
            DefArr.Add(MakeShared<FJsonValueString>(S));
        }
        FieldObj->SetArrayField(TEXT("DefaultArray"), DefArr);

        FieldsArray.Add(MakeShared<FJsonValueObject>(FieldObj));
    }

    RootObj->SetArrayField(TEXT("Fields"), FieldsArray);

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutText);
    if (!FJsonSerializer::Serialize(RootObj, Writer))
    {
        OutError = TEXT("Error serializando JSON de schema");
        return false;
    }

    return true;
}
