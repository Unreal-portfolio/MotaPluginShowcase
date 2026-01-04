// MotaEzTomlSerializer.cpp

#include "MotaEzTomlSerializer.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#define TOML_EXCEPTIONS 1

THIRD_PARTY_INCLUDES_START
#include "toml.hpp"
THIRD_PARTY_INCLUDES_END

#include <string>
#include <sstream>

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


    EMotaEzFieldType DeduceTypeFromNode(const toml::node& Node, bool bIsArray)
    {
        if (bIsArray)
        {
            auto Arr = Node.as_array();
            if (!Arr || Arr->empty())
                return EMotaEzFieldType::None;

            const toml::node& Elem = *Arr->get(0);

            if (Elem.is_integer())        return EMotaEzFieldType::Int;
            if (Elem.is_floating_point()) return EMotaEzFieldType::Float;
            if (Elem.is_boolean())        return EMotaEzFieldType::Bool;
            if (Elem.is_string())         return EMotaEzFieldType::String;

            return EMotaEzFieldType::None;
        }
        else
        {
            if (Node.is_integer())        return EMotaEzFieldType::Int;
            if (Node.is_floating_point()) return EMotaEzFieldType::Float;
            if (Node.is_boolean())        return EMotaEzFieldType::Bool;
            if (Node.is_string())         return EMotaEzFieldType::String;

            return EMotaEzFieldType::None;
        }
    }


    bool InternalLoadFromTomlText(
        const FString& InTomlText,
        TMap<FString, FMotaEzValue>& OutValues,
        FString& OutError)
    {
        OutValues.Reset();
        OutError.Reset();

        std::string Source = FStringToStd(InTomlText);

        toml::table Tbl;
        try
        {
            Tbl = toml::parse(Source); // con TOML_EXCEPTIONS = 1
        }
        catch (const toml::parse_error& Err)
        {
            std::string ErrDesc = std::string(Err.description());
            OutError = FString::Printf(
                TEXT("TOML parse error: %hs (line %d)"),
                ErrDesc.c_str(),
                (int32)Err.source().begin.line
            );

            UE_LOG(LogTemp, Error, TEXT("%s"), *OutError);
            return false;
        }

        for (auto&& [Key, Node] : Tbl)
        {
            const std::string KeyStd = std::string(Key.str());
            const FString KeyF = StdToFString(KeyStd);

            FMotaEzValue Value;
            Value.ScalarValue.Reset();
            Value.ArrayValues.Reset();

            if (auto Arr = Node.as_array())
            {
                // ---------- ARRAY ----------
                const int32 Num = (int32)Arr->size();
                if (Num == 0)
                {
                    continue;
                }

                Value.ArrayValues.Reserve(Num);
                Value.Type = DeduceTypeFromNode(Node, /*bIsArray=*/true);

                for (const toml::node& Elem : *Arr)
                {
                    if (auto Iv = Elem.value<int64_t>())
                    {
                        Value.ArrayValues.Add(
                            FString::Printf(TEXT("%lld"), (long long)*Iv)
                        );
                    }
                    else if (auto Fv = Elem.value<double>())
                    {
                        Value.ArrayValues.Add(
                            LexToString((float)*Fv)
                        );
                    }
                    else if (auto Bv = Elem.value<bool>())
                    {
                        Value.ArrayValues.Add(*Bv ? TEXT("true") : TEXT("false"));
                    }
                    else if (auto Sv = Elem.value<std::string>())
                    {
                        Value.ArrayValues.Add(StdToFString(*Sv));
                    }
                    else
                    {
                        // tipo no soportado: lo ignoramos
                    }
                }
            }
            else
            {
                Value.Type = DeduceTypeFromNode(Node, /*bIsArray=*/false);

                if (auto Iv = Node.value<int64_t>())
                {
                    Value.ScalarValue =
                        FString::Printf(TEXT("%lld"), (long long)*Iv);
                }
                else if (auto Fv = Node.value<double>())
                {
                    Value.ScalarValue = LexToString((float)*Fv);
                }
                else if (auto Bv = Node.value<bool>())
                {
                    Value.ScalarValue = *Bv ? TEXT("true") : TEXT("false");
                }
                else if (auto Sv = Node.value<std::string>())
                {
                    Value.ScalarValue = StdToFString(*Sv);
                    
                    // Auto-detect Vector and Rotator types based on key suffix or content pattern
                    if (KeyF.EndsWith(TEXT("_Pos")) || KeyF.EndsWith(TEXT("Position")) || 
                        KeyF.Contains(TEXT("Location")) || KeyF.Contains(TEXT("Vector")))
                    {
                        // Check if string contains comma-separated floats (Vector pattern)
                        TArray<FString> Parts;
                        Value.ScalarValue.ParseIntoArray(Parts, TEXT(","), true);
                        if (Parts.Num() == 3)
                        {
                            Value.Type = EMotaEzFieldType::Vector;
                        }
                    }
                    else if (KeyF.EndsWith(TEXT("_Rot")) || KeyF.EndsWith(TEXT("Rotation")) || 
                             KeyF.Contains(TEXT("Rotator")))
                    {
                        // Check if string contains comma-separated floats (Rotator pattern)
                        TArray<FString> Parts;
                        Value.ScalarValue.ParseIntoArray(Parts, TEXT(","), true);
                        if (Parts.Num() == 3)
                        {
                            Value.Type = EMotaEzFieldType::Rotator;
                        }
                    }
                }
                else
                {
                    continue;
                }
            }

            OutValues.Add(KeyF, MoveTemp(Value));
        }

        return true;
    }
    
    bool InternalSaveToTomlText(
        FString& OutTomlText,
        const TMap<FString, FMotaEzValue>& InValues,
        FString& OutError)
    {
        OutTomlText.Reset();
        OutError.Reset();

        toml::table Tbl;

        for (const TPair<FString, FMotaEzValue>& Pair : InValues)
        {
            const FString& KeyF = Pair.Key;
            const FMotaEzValue& Val = Pair.Value;

            std::string KeyStd = FStringToStd(KeyF);

            if (Val.IsArray())
            {
                toml::array Arr;

                switch (Val.Type)
                {
                case EMotaEzFieldType::Int:
                    for (const FString& S : Val.ArrayValues)
                    {
                        const int64_t V = (int64_t)FCString::Atoi(*S);
                        Arr.push_back(V);
                    }
                    break;

                case EMotaEzFieldType::Float:
                    for (const FString& S : Val.ArrayValues)
                    {
                        const double V = (double)FCString::Atof(*S);
                        Arr.push_back(V);
                    }
                    break;

                case EMotaEzFieldType::Bool:
                    for (const FString& S : Val.ArrayValues)
                    {
                        const bool B =
                            S.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
                            S.Equals(TEXT("1"), ESearchCase::IgnoreCase);
                        Arr.push_back(B);
                    }
                    break;

                case EMotaEzFieldType::String:
                default:
                    for (const FString& S : Val.ArrayValues)
                    {
                        Arr.push_back(FStringToStd(S));
                    }
                    break;
                }

                Tbl.insert_or_assign(std::move(KeyStd), std::move(Arr));
            }
            else
            {
                switch (Val.Type)
                {
                case EMotaEzFieldType::Int:
                {
                    const int64_t V = (int64_t)FCString::Atoi(*Val.ScalarValue);
                    Tbl.insert_or_assign(std::move(KeyStd), V);
                    break;
                }
                case EMotaEzFieldType::Float:
                {
                    const double V = (double)FCString::Atof(*Val.ScalarValue);
                    Tbl.insert_or_assign(std::move(KeyStd), V);
                    break;
                }
                case EMotaEzFieldType::Bool:
                {
                    const bool B =
                        Val.ScalarValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
                        Val.ScalarValue.Equals(TEXT("1"),   ESearchCase::IgnoreCase);
                    Tbl.insert_or_assign(std::move(KeyStd), B);
                    break;
                }
                case EMotaEzFieldType::String:
                default:
                {
                    Tbl.insert_or_assign(std::move(KeyStd),
                                         FStringToStd(Val.ScalarValue));
                    break;
                }
                }
            }
        }

        std::ostringstream OSS;
        OSS << Tbl;           // toml++ define operator<<(ostream&, table)

        OutTomlText = StdToFString(OSS.str());
        return true;
    }
} // namespace anónimo


bool UMotaEzTomlSerializer::LoadConfig(
    const FString& InText,
    TMap<FString, FMotaEzValue>& OutData,
    FString& OutError)
{
    return InternalLoadFromTomlText(InText, OutData, OutError);
}

bool UMotaEzTomlSerializer::SaveConfig(
    const TMap<FString, FMotaEzValue>& InData,
    FString& OutText,
    FString& OutError)
{
    return InternalSaveToTomlText(OutText, InData, OutError);
}

bool UMotaEzTomlSerializer::LoadSchema(
    const FString& InText,
    FMotaEzSaveSchema& OutSchema,
    FString& OutError)
{
    OutError.Reset();
    OutSchema.Fields.Reset();

    const std::string TomlStd = FStringToStd(InText);

    try
    {
        toml::table Tbl = toml::parse(TomlStd);

        // Esperamos un array "Fields" con la definición de cada campo
        auto FieldsArray = Tbl["Fields"].as_array();
        if (!FieldsArray)
        {
            // Schema vacío es válido
            return true;
        }

        for (const auto& ElemNode : *FieldsArray)
        {
            auto FieldTable = ElemNode.as_table();
            if (!FieldTable)
            {
                continue;
            }

            FMotaEzFieldDescriptor Desc;
            
            auto NameNode = (*FieldTable)["Name"].as_string();
            if (!NameNode)
            {
                continue;
            }
            std::string NameStd = std::string(NameNode->get());
            Desc.Name = StdToFString(NameStd);

            if (Desc.Name.IsEmpty())
            {
                continue;
            }

            // Type
            auto TypeNode = (*FieldTable)["Type"].as_string();
            if (TypeNode)
            {
                std::string TypeStd = std::string(TypeNode->get());
                FString TypeStr = StdToFString(TypeStd);
                
                if (TypeStr.Equals(TEXT("Int"), ESearchCase::IgnoreCase))
                    Desc.Type = EMotaEzFieldType::Int;
                else if (TypeStr.Equals(TEXT("Float"), ESearchCase::IgnoreCase))
                    Desc.Type = EMotaEzFieldType::Float;
                else if (TypeStr.Equals(TEXT("Bool"), ESearchCase::IgnoreCase))
                    Desc.Type = EMotaEzFieldType::Bool;
                else if (TypeStr.Equals(TEXT("String"), ESearchCase::IgnoreCase))
                    Desc.Type = EMotaEzFieldType::String;
                else if (TypeStr.Equals(TEXT("Vector"), ESearchCase::IgnoreCase))
                    Desc.Type = EMotaEzFieldType::Vector;
                else if (TypeStr.Equals(TEXT("Rotator"), ESearchCase::IgnoreCase))
                    Desc.Type = EMotaEzFieldType::Rotator;
            }

            // IsArray
            auto IsArrayNode = (*FieldTable)["IsArray"].as_boolean();
            if (IsArrayNode)
            {
                Desc.bIsArray = IsArrayNode->get();
            }

            // DefaultValue
            auto DefValNode = (*FieldTable)["DefaultValue"].as_string();
            if (DefValNode)
            {
                std::string DefValStd = std::string(DefValNode->get());
                Desc.DefaultValue = StdToFString(DefValStd);
            }

            // DefaultArray
            auto DefArrNode = (*FieldTable)["DefaultArray"].as_array();
            if (DefArrNode)
            {
                for (const auto& ArrElem : *DefArrNode)
                {
                    auto StrNode = ArrElem.as_string();
                    if (StrNode)
                    {
                        std::string ElemStd = std::string(StrNode->get());
                        Desc.DefaultArray.Add(StdToFString(ElemStd));
                    }
                }
            }

            OutSchema.Fields.Add(Desc.Name, Desc);
        }

        return true;
    }
    catch (const toml::parse_error& Err)
    {
        std::string ErrDesc = std::string(Err.description());
        OutError = FString::Printf(
            TEXT("TOML schema parse error: %hs (line %d)"),
            ErrDesc.c_str(),
            (int32)Err.source().begin.line
        );
        return false;
    }
}

bool UMotaEzTomlSerializer::SaveSchema(
    const FMotaEzSaveSchema& InSchema,
    FString& OutText,
    FString& OutError)
{
    OutError.Reset();
    OutText.Reset();

    try
    {
        toml::table Tbl;
        toml::array FieldsArray;

        for (const auto& Pair : InSchema.Fields)
        {
            const FMotaEzFieldDescriptor& Desc = Pair.Value;

            toml::table FieldTable;
            
            // Name
            FieldTable.insert_or_assign("Name", FStringToStd(Desc.Name));

            // Type
            FString TypeStr;
            switch (Desc.Type)
            {
            case EMotaEzFieldType::Int:     TypeStr = TEXT("Int"); break;
            case EMotaEzFieldType::Float:   TypeStr = TEXT("Float"); break;
            case EMotaEzFieldType::Bool:    TypeStr = TEXT("Bool"); break;
            case EMotaEzFieldType::String:  TypeStr = TEXT("String"); break;
            case EMotaEzFieldType::Vector:  TypeStr = TEXT("Vector"); break;
            case EMotaEzFieldType::Rotator: TypeStr = TEXT("Rotator"); break;
            default:                        TypeStr = TEXT("None"); break;
            }
            FieldTable.insert_or_assign("Type", FStringToStd(TypeStr));

            // IsArray
            FieldTable.insert_or_assign("IsArray", Desc.bIsArray);

            // DefaultValue
            FieldTable.insert_or_assign("DefaultValue", FStringToStd(Desc.DefaultValue));

            // DefaultArray
            toml::array DefArray;
            for (const FString& Str : Desc.DefaultArray)
            {
                DefArray.push_back(FStringToStd(Str));
            }
            FieldTable.insert_or_assign("DefaultArray", DefArray);

            FieldsArray.push_back(FieldTable);
        }

        Tbl.insert_or_assign("Fields", FieldsArray);

        std::ostringstream OSS;
        OSS << Tbl;
        OutText = StdToFString(OSS.str());

        return true;
    }
    catch (const std::exception& Ex)
    {
        OutError = FString::Printf(TEXT("TOML schema save error: %hs"), Ex.what());
        return false;
    }
}

