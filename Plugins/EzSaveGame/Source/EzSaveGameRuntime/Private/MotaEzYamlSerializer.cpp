// MotaEzYamlSerializer.cpp

#include "MotaEzYamlSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

THIRD_PARTY_INCLUDES_START
#include "yaml-cpp/yaml.h"
THIRD_PARTY_INCLUDES_END

namespace
{
    FORCEINLINE std::string FStringToStd(const FString& S)
    {
        return std::string(TCHAR_TO_UTF8(*S));
    }

    FORCEINLINE FString StdToFString(const std::string& S)
    {
        return FString(UTF8_TO_TCHAR(S.c_str()));
    }

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

bool UMotaEzYamlSerializer::LoadConfig(
    const FString& InText,
    TMap<FString, FMotaEzValue>& OutData,
    FString& OutError)
{
    OutError.Reset();
    OutData.Reset();

    try
    {
        YAML::Node Root = YAML::Load(FStringToStd(InText));

        for (const auto& Pair : Root)
        {
            std::string KeyStd = Pair.first.as<std::string>();
            FString Key = StdToFString(KeyStd);
            const YAML::Node& ValNode = Pair.second;

            FMotaEzValue Val;

            if (ValNode.IsSequence())
            {
                Val.bIsArray = true;
                
                if (ValNode.size() > 0)
                {
                    const YAML::Node& FirstElem = ValNode[0];
                    if (FirstElem.IsScalar())
                    {
                        std::string Str = FirstElem.as<std::string>();
                        
                        if (Str == "true" || Str == "false")
                        {
                            Val.Type = EMotaEzFieldType::Bool;
                        }
                        else if (Str.find('.') != std::string::npos)
                        {
                            Val.Type = EMotaEzFieldType::Float;
                        }
                        else
                        {
                            bool bIsInt = true;
                            for (char c : Str)
                            {
                                if (!isdigit(c) && c != '-' && c != '+')
                                {
                                    bIsInt = false;
                                    break;
                                }
                            }
                            Val.Type = bIsInt ? EMotaEzFieldType::Int : EMotaEzFieldType::String;
                        }
                    }
                }

                for (const auto& Elem : ValNode)
                {
                    if (Elem.IsScalar())
                    {
                        Val.ArrayValues.Add(StdToFString(Elem.as<std::string>()));
                    }
                }
            }
            else if (ValNode.IsScalar())
            {
                Val.bIsArray = false;
                std::string ValStr = ValNode.as<std::string>();
                
                if (ValStr == "true" || ValStr == "false")
                {
                    Val.Type = EMotaEzFieldType::Bool;
                }
                else if (ValStr.find('.') != std::string::npos)
                {
                    Val.Type = EMotaEzFieldType::Float;
                }
                else
                {
                    bool bIsInt = true;
                    for (char c : ValStr)
                    {
                        if (!isdigit(c) && c != '-' && c != '+')
                        {
                            bIsInt = false;
                            break;
                        }
                    }
                    Val.Type = bIsInt ? EMotaEzFieldType::Int : EMotaEzFieldType::String;
                }

                Val.ScalarValue = StdToFString(ValStr);
                
                // Auto-detect Vector and Rotator types based on key suffix
                if (Key.EndsWith(TEXT("_Pos")) || Key.EndsWith(TEXT("Position")) || 
                    Key.Contains(TEXT("Location")) || Key.Contains(TEXT("Vector")))
                {
                    TArray<FString> Parts;
                    Val.ScalarValue.ParseIntoArray(Parts, TEXT(","), true);
                    if (Parts.Num() == 3)
                    {
                        Val.Type = EMotaEzFieldType::Vector;
                    }
                }
                else if (Key.EndsWith(TEXT("_Rot")) || Key.EndsWith(TEXT("Rotation")) || 
                         Key.Contains(TEXT("Rotator")))
                {
                    TArray<FString> Parts;
                    Val.ScalarValue.ParseIntoArray(Parts, TEXT(","), true);
                    if (Parts.Num() == 3)
                    {
                        Val.Type = EMotaEzFieldType::Rotator;
                    }
                }
            }

            OutData.Add(Key, Val);
        }

        return true;
    }
    catch (const YAML::Exception& Ex)
    {
        OutError = FString::Printf(TEXT("YAML parse error: %hs"), Ex.what());
        return false;
    }
}

bool UMotaEzYamlSerializer::SaveConfig(
    const TMap<FString, FMotaEzValue>& InData,
    FString& OutText,
    FString& OutError)
{
    OutError.Reset();
    OutText.Reset();

    try
    {
        YAML::Emitter Out;
        Out << YAML::BeginMap;

        for (const auto& Pair : InData)
        {
            const FString& Key = Pair.Key;
            const FMotaEzValue& Val = Pair.Value;

            Out << YAML::Key << FStringToStd(Key);

            if (Val.bIsArray)
            {
                Out << YAML::Value << YAML::BeginSeq;
                for (const FString& Str : Val.ArrayValues)
                {
                    Out << FStringToStd(Str);
                }
                Out << YAML::EndSeq;
            }
            else
            {
                Out << YAML::Value << FStringToStd(Val.ScalarValue);
            }
        }

        Out << YAML::EndMap;

        OutText = StdToFString(Out.c_str());
        return true;
    }
    catch (const YAML::Exception& Ex)
    {
        OutError = FString::Printf(TEXT("YAML save error: %hs"), Ex.what());
        return false;
    }
}

bool UMotaEzYamlSerializer::LoadSchema(
    const FString& InText,
    FMotaEzSaveSchema& OutSchema,
    FString& OutError)
{
    OutError.Reset();
    OutSchema.Fields.Reset();

    try
    {
        YAML::Node Root = YAML::Load(FStringToStd(InText));

        if (!Root["Fields"] || !Root["Fields"].IsSequence())
        {
            return true;
        }

        const YAML::Node& FieldsArray = Root["Fields"];

        for (const auto& FieldNode : FieldsArray)
        {
            FMotaEzFieldDescriptor Desc;

            if (FieldNode["Name"])
            {
                Desc.Name = StdToFString(FieldNode["Name"].as<std::string>());
            }

            if (Desc.Name.IsEmpty())
            {
                continue;
            }

            if (FieldNode["Type"])
            {
                FString TypeStr = StdToFString(FieldNode["Type"].as<std::string>());
                Desc.Type = ParseTypeFromString(TypeStr);
            }

            if (FieldNode["IsArray"])
            {
                Desc.bIsArray = FieldNode["IsArray"].as<bool>();
            }

            if (FieldNode["DefaultValue"])
            {
                Desc.DefaultValue = StdToFString(FieldNode["DefaultValue"].as<std::string>());
            }

            if (FieldNode["DefaultArray"] && FieldNode["DefaultArray"].IsSequence())
            {
                for (const auto& Elem : FieldNode["DefaultArray"])
                {
                    Desc.DefaultArray.Add(StdToFString(Elem.as<std::string>()));
                }
            }

            OutSchema.Fields.Add(Desc.Name, Desc);
        }

        return true;
    }
    catch (const YAML::Exception& Ex)
    {
        OutError = FString::Printf(TEXT("YAML schema parse error: %hs"), Ex.what());
        return false;
    }
}

bool UMotaEzYamlSerializer::SaveSchema(
    const FMotaEzSaveSchema& InSchema,
    FString& OutText,
    FString& OutError)
{
    OutError.Reset();
    OutText.Reset();

    try
    {
        YAML::Emitter Out;
        Out << YAML::BeginMap;
        Out << YAML::Key << "Fields";
        Out << YAML::Value << YAML::BeginSeq;

        for (const auto& Pair : InSchema.Fields)
        {
            const FMotaEzFieldDescriptor& Desc = Pair.Value;

            Out << YAML::BeginMap;
            Out << YAML::Key << "Name" << YAML::Value << FStringToStd(Desc.Name);
            Out << YAML::Key << "Type" << YAML::Value << FStringToStd(TypeToString(Desc.Type));
            Out << YAML::Key << "IsArray" << YAML::Value << Desc.bIsArray;
            Out << YAML::Key << "DefaultValue" << YAML::Value << FStringToStd(Desc.DefaultValue);
            
            Out << YAML::Key << "DefaultArray" << YAML::Value << YAML::BeginSeq;
            for (const FString& Str : Desc.DefaultArray)
            {
                Out << FStringToStd(Str);
            }
            Out << YAML::EndSeq;

            Out << YAML::EndMap;
        }

        Out << YAML::EndSeq;
        Out << YAML::EndMap;

        OutText = StdToFString(Out.c_str());
        return true;
    }
    catch (const YAML::Exception& Ex)
    {
        OutError = FString::Printf(TEXT("YAML schema save error: %hs"), Ex.what());
        return false;
    }
}

