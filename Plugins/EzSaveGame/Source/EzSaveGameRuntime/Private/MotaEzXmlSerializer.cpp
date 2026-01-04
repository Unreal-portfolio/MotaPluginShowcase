// MotaEzXmlSerializer.cpp

#include "MotaEzXmlSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include <string>

THIRD_PARTY_INCLUDES_START
#include "tinyxml2.h"
THIRD_PARTY_INCLUDES_END

using namespace tinyxml2;

namespace
{
    FORCEINLINE const char* FStringToCStr(const FString& S, std::string& Buffer)
    {
        Buffer = std::string(TCHAR_TO_UTF8(*S));
        return Buffer.c_str();
    }

    FORCEINLINE FString CStrToFString(const char* S)
    {
        return S ? FString(UTF8_TO_TCHAR(S)) : FString();
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

    EMotaEzFieldType DeduceTypeFromString(const FString& Str)
    {
        if (Str.Equals(TEXT("true"), ESearchCase::IgnoreCase) || 
            Str.Equals(TEXT("false"), ESearchCase::IgnoreCase))
        {
            return EMotaEzFieldType::Bool;
        }
        
        if (Str.Contains(TEXT(".")))
        {
            return EMotaEzFieldType::Float;
        }

        bool bIsInt = true;
        for (TCHAR c : Str)
        {
            if (!FChar::IsDigit(c) && c != '-' && c != '+')
            {
                bIsInt = false;
                break;
            }
        }

        return bIsInt ? EMotaEzFieldType::Int : EMotaEzFieldType::String;
    }
}

bool UMotaEzXmlSerializer::LoadConfig(
    const FString& InText,
    TMap<FString, FMotaEzValue>& OutData,
    FString& OutError)
{
    OutError.Reset();
    OutData.Reset();

    XMLDocument Doc;
    std::string XmlStd = std::string(TCHAR_TO_UTF8(*InText));
    
    XMLError Err = Doc.Parse(XmlStd.c_str());
    if (Err != XML_SUCCESS)
    {
        OutError = FString::Printf(TEXT("XML parse error: %d"), (int32)Err);
        return false;
    }

    XMLElement* Root = Doc.FirstChildElement("Config");
    if (!Root)
    {
        OutError = TEXT("XML root element 'Config' not found");
        return false;
    }

    for (XMLElement* Field = Root->FirstChildElement("Field"); Field != nullptr; Field = Field->NextSiblingElement("Field"))
    {
        const char* KeyCStr = Field->Attribute("Name");
        if (!KeyCStr)
        {
            continue;
        }

        FString Key = CStrToFString(KeyCStr);
        FMotaEzValue Val;

        // Check si es array
        XMLElement* ArrayElem = Field->FirstChildElement("Array");
        if (ArrayElem)
        {
            Val.bIsArray = true;

            for (XMLElement* Item = ArrayElem->FirstChildElement("Item"); Item != nullptr; Item = Item->NextSiblingElement("Item"))
            {
                const char* TextCStr = Item->GetText();
                FString ItemText = CStrToFString(TextCStr);
                Val.ArrayValues.Add(ItemText);

                // Deducir tipo del primer elemento
                if (Val.Type == EMotaEzFieldType::None)
                {
                    Val.Type = DeduceTypeFromString(ItemText);
                }
            }
        }
        else
        {
            Val.bIsArray = false;
            const char* TextCStr = Field->GetText();
            Val.ScalarValue = CStrToFString(TextCStr);
            Val.Type = DeduceTypeFromString(Val.ScalarValue);
            
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

bool UMotaEzXmlSerializer::SaveConfig(
    const TMap<FString, FMotaEzValue>& InData,
    FString& OutText,
    FString& OutError)
{
    OutError.Reset();
    OutText.Reset();

    XMLDocument Doc;
    XMLElement* Root = Doc.NewElement("Config");
    Doc.InsertFirstChild(Root);

    for (const auto& Pair : InData)
    {
        const FString& Key = Pair.Key;
        const FMotaEzValue& Val = Pair.Value;

        std::string KeyBuffer;
        XMLElement* FieldElem = Doc.NewElement("Field");
        FieldElem->SetAttribute("Name", FStringToCStr(Key, KeyBuffer));

        if (Val.bIsArray)
        {
            XMLElement* ArrayElem = Doc.NewElement("Array");
            
            for (const FString& Str : Val.ArrayValues)
            {
                XMLElement* ItemElem = Doc.NewElement("Item");
                std::string ItemBuffer;
                ItemElem->SetText(FStringToCStr(Str, ItemBuffer));
                ArrayElem->InsertEndChild(ItemElem);
            }

            FieldElem->InsertEndChild(ArrayElem);
        }
        else
        {
            std::string ValBuffer;
            FieldElem->SetText(FStringToCStr(Val.ScalarValue, ValBuffer));
        }

        Root->InsertEndChild(FieldElem);
    }

    XMLPrinter Printer;
    Doc.Print(&Printer);
    OutText = CStrToFString(Printer.CStr());

    return true;
}

bool UMotaEzXmlSerializer::LoadSchema(
    const FString& InText,
    FMotaEzSaveSchema& OutSchema,
    FString& OutError)
{
    OutError.Reset();
    OutSchema.Fields.Reset();

    XMLDocument Doc;
    std::string XmlStd = std::string(TCHAR_TO_UTF8(*InText));
    
    XMLError Err = Doc.Parse(XmlStd.c_str());
    if (Err != XML_SUCCESS)
    {
        OutError = FString::Printf(TEXT("XML schema parse error: %d"), (int32)Err);
        return false;
    }

    XMLElement* Root = Doc.FirstChildElement("Schema");
    if (!Root)
    {
        return true;
    }

    XMLElement* Fields = Root->FirstChildElement("Fields");
    if (!Fields)
    {
        return true;
    }

    for (XMLElement* FieldElem = Fields->FirstChildElement("Field"); FieldElem != nullptr; FieldElem = FieldElem->NextSiblingElement("Field"))
    {
        FMotaEzFieldDescriptor Desc;

        const char* NameCStr = FieldElem->Attribute("Name");
        if (!NameCStr)
        {
            continue;
        }

        Desc.Name = CStrToFString(NameCStr);

        const char* TypeCStr = FieldElem->Attribute("Type");
        if (TypeCStr)
        {
            Desc.Type = ParseTypeFromString(CStrToFString(TypeCStr));
        }

        const char* IsArrayCStr = FieldElem->Attribute("IsArray");
        if (IsArrayCStr)
        {
            FString IsArrayStr = CStrToFString(IsArrayCStr);
            Desc.bIsArray = IsArrayStr.Equals(TEXT("true"), ESearchCase::IgnoreCase) || IsArrayStr.Equals(TEXT("1"));
        }

        const char* DefValCStr = FieldElem->Attribute("DefaultValue");
        if (DefValCStr)
        {
            Desc.DefaultValue = CStrToFString(DefValCStr);
        }

        XMLElement* DefArrayElem = FieldElem->FirstChildElement("DefaultArray");
        if (DefArrayElem)
        {
            for (XMLElement* ItemElem = DefArrayElem->FirstChildElement("Item"); ItemElem != nullptr; ItemElem = ItemElem->NextSiblingElement("Item"))
            {
                const char* TextCStr = ItemElem->GetText();
                Desc.DefaultArray.Add(CStrToFString(TextCStr));
            }
        }

        OutSchema.Fields.Add(Desc.Name, Desc);
    }

    return true;
}

bool UMotaEzXmlSerializer::SaveSchema(
    const FMotaEzSaveSchema& InSchema,
    FString& OutText,
    FString& OutError)
{
    OutError.Reset();
    OutText.Reset();

    XMLDocument Doc;
    XMLElement* Root = Doc.NewElement("Schema");
    Doc.InsertFirstChild(Root);

    XMLElement* FieldsElem = Doc.NewElement("Fields");
    Root->InsertEndChild(FieldsElem);

    for (const auto& Pair : InSchema.Fields)
    {
        const FMotaEzFieldDescriptor& Desc = Pair.Value;

        std::string NameBuffer, TypeBuffer, DefValBuffer;
        XMLElement* FieldElem = Doc.NewElement("Field");
        FieldElem->SetAttribute("Name", FStringToCStr(Desc.Name, NameBuffer));
        FieldElem->SetAttribute("Type", FStringToCStr(TypeToString(Desc.Type), TypeBuffer));
        FieldElem->SetAttribute("IsArray", Desc.bIsArray ? "true" : "false");
        FieldElem->SetAttribute("DefaultValue", FStringToCStr(Desc.DefaultValue, DefValBuffer));

        if (Desc.DefaultArray.Num() > 0)
        {
            XMLElement* DefArrayElem = Doc.NewElement("DefaultArray");
            
            for (const FString& Str : Desc.DefaultArray)
            {
                XMLElement* ItemElem = Doc.NewElement("Item");
                std::string ItemBuffer;
                ItemElem->SetText(FStringToCStr(Str, ItemBuffer));
                DefArrayElem->InsertEndChild(ItemElem);
            }

            FieldElem->InsertEndChild(DefArrayElem);
        }

        FieldsElem->InsertEndChild(FieldElem);
    }

    XMLPrinter Printer;
    Doc.Print(&Printer);
    OutText = CStrToFString(Printer.CStr());

    return true;
}

