#include "MotaEzSaveData.h"
#include "Misc/Char.h"
#include "Containers/UnrealString.h"

namespace
{
    FORCEINLINE bool StringToBool(const FString& In)
    {
        return In.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
               In.Equals(TEXT("1"), ESearchCase::IgnoreCase)   ||
               In.Equals(TEXT("yes"), ESearchCase::IgnoreCase);
    }

    FORCEINLINE FString BoolToString(bool b)
    {
        return b ? TEXT("true") : TEXT("false");
    }

    FORCEINLINE FVector StringToVector(const FString& In, const FVector& Default)
    {
        TArray<FString> Parts;
        In.ParseIntoArray(Parts, TEXT(","), true);

        if (Parts.Num() != 3)
        {
            return Default;
        }

        const float X = FCString::Atof(*Parts[0]);
        const float Y = FCString::Atof(*Parts[1]);
        const float Z = FCString::Atof(*Parts[2]);
        return FVector(X, Y, Z);
    }

    FORCEINLINE FString VectorToString(const FVector& V)
    {
        return FString::Printf(TEXT("%f,%f,%f"), V.X, V.Y, V.Z);
    }

    FORCEINLINE FRotator StringToRotator(const FString& In, const FRotator& Default)
    {
        TArray<FString> Parts;
        In.ParseIntoArray(Parts, TEXT(","), true);

        if (Parts.Num() != 3)
        {
            return Default;
        }

        const float Pitch = FCString::Atof(*Parts[0]);
        const float Yaw   = FCString::Atof(*Parts[1]);
        const float Roll  = FCString::Atof(*Parts[2]);
        return FRotator(Pitch, Yaw, Roll);
    }

    FORCEINLINE FString RotatorToString(const FRotator& R)
    {
        return FString::Printf(TEXT("%f,%f,%f"), R.Pitch, R.Yaw, R.Roll);
    }

    FORCEINLINE FVector ParseVectorElem(const FString& In)
    {
        return StringToVector(In, FVector::ZeroVector);
    }

    FORCEINLINE FRotator ParseRotatorElem(const FString& In)
    {
        return StringToRotator(In, FRotator::ZeroRotator);
    }
}


int32 UMotaEzSaveData::GetInt(const FString& Key, int32 DefaultValue) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || Found->IsArray() || Found->Type != EMotaEzFieldType::Int)
    {
        return DefaultValue;
    }

    return FCString::Atoi(*Found->ScalarValue);
}

float UMotaEzSaveData::GetFloat(const FString& Key, float DefaultValue) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || Found->IsArray() || Found->Type != EMotaEzFieldType::Float)
    {
        return DefaultValue;
    }

    return FCString::Atof(*Found->ScalarValue);
}

bool UMotaEzSaveData::GetBool(const FString& Key, bool DefaultValue) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || Found->IsArray() || Found->Type != EMotaEzFieldType::Bool)
    {
        return DefaultValue;
    }

    return StringToBool(Found->ScalarValue);
}

FString UMotaEzSaveData::GetString(const FString& Key, const FString& DefaultValue) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || Found->IsArray() || Found->Type != EMotaEzFieldType::String)
    {
        return DefaultValue;
    }

    return Found->ScalarValue;
}

FVector UMotaEzSaveData::GetVector(const FString& Key, const FVector& DefaultValue) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || Found->IsArray() || Found->Type != EMotaEzFieldType::Vector)
    {
        return DefaultValue;
    }

    return StringToVector(Found->ScalarValue, DefaultValue);
}

FRotator UMotaEzSaveData::GetRotator(const FString& Key, const FRotator& DefaultValue) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || Found->IsArray() || Found->Type != EMotaEzFieldType::Rotator)
    {
        return DefaultValue;
    }

    return StringToRotator(Found->ScalarValue, DefaultValue);
}

void UMotaEzSaveData::SetInt(const FString& Key, int32 Value)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Int;
    Entry.ScalarValue = LexToString(Value);
    Entry.ArrayValues.Reset();
}

void UMotaEzSaveData::SetFloat(const FString& Key, float Value)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Float;
    Entry.ScalarValue = LexToString(Value);
    Entry.ArrayValues.Reset();
}

void UMotaEzSaveData::SetBool(const FString& Key, bool Value)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Bool;
    Entry.ScalarValue = BoolToString(Value);
    Entry.ArrayValues.Reset();
}

void UMotaEzSaveData::SetString(const FString& Key, const FString& Value)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::String;
    Entry.ScalarValue = Value;
    Entry.ArrayValues.Reset();
}

void UMotaEzSaveData::SetVector(const FString& Key, const FVector& Value)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Vector;
    Entry.ScalarValue = VectorToString(Value);
    Entry.ArrayValues.Reset();
}

void UMotaEzSaveData::SetRotator(const FString& Key, const FRotator& Value)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Rotator;
    Entry.ScalarValue = RotatorToString(Value);
    Entry.ArrayValues.Reset();
}

TArray<int32> UMotaEzSaveData::GetIntArray(const FString& Key) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    TArray<int32> Result;

    if (!Found || !Found->IsArray() || Found->Type != EMotaEzFieldType::Int)
    {
        return Result;
    }

    Result.Reserve(Found->ArrayValues.Num());
    for (const FString& S : Found->ArrayValues)
    {
        Result.Add(FCString::Atoi(*S));
    }
    return Result;
}

TArray<float> UMotaEzSaveData::GetFloatArray(const FString& Key) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    TArray<float> Result;

    if (!Found || !Found->IsArray() || Found->Type != EMotaEzFieldType::Float)
    {
        return Result;
    }

    Result.Reserve(Found->ArrayValues.Num());
    for (const FString& S : Found->ArrayValues)
    {
        Result.Add(FCString::Atof(*S));
    }
    return Result;
}

TArray<bool> UMotaEzSaveData::GetBoolArray(const FString& Key) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    TArray<bool> Result;

    if (!Found || !Found->IsArray() || Found->Type != EMotaEzFieldType::Bool)
    {
        return Result;
    }

    Result.Reserve(Found->ArrayValues.Num());
    for (const FString& S : Found->ArrayValues)
    {
        Result.Add(StringToBool(S));
    }
    return Result;
}

TArray<FString> UMotaEzSaveData::GetStringArray(const FString& Key) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    if (!Found || !Found->IsArray() || Found->Type != EMotaEzFieldType::String)
    {
        return {};
    }

    return Found->ArrayValues;
}

TArray<FVector> UMotaEzSaveData::GetVectorArray(const FString& Key) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    TArray<FVector> Result;

    if (!Found || !Found->IsArray() || Found->Type != EMotaEzFieldType::Vector)
    {
        return Result;
    }

    Result.Reserve(Found->ArrayValues.Num());
    for (const FString& S : Found->ArrayValues)
    {
        Result.Add(ParseVectorElem(S));
    }
    return Result;
}

TArray<FRotator> UMotaEzSaveData::GetRotatorArray(const FString& Key) const
{
    const FMotaEzValue* Found = Data.Find(Key);
    TArray<FRotator> Result;

    if (!Found || !Found->IsArray() || Found->Type != EMotaEzFieldType::Rotator)
    {
        return Result;
    }

    Result.Reserve(Found->ArrayValues.Num());
    for (const FString& S : Found->ArrayValues)
    {
        Result.Add(ParseRotatorElem(S));
    }
    return Result;
}

void UMotaEzSaveData::SetIntArray(const FString& Key, const TArray<int32>& Values)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Int;
    Entry.ScalarValue.Reset();
    Entry.ArrayValues.Reset();

    Entry.ArrayValues.Reserve(Values.Num());
    for (int32 V : Values)
    {
        Entry.ArrayValues.Add(LexToString(V));
    }
}

void UMotaEzSaveData::SetFloatArray(const FString& Key, const TArray<float>& Values)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Float;
    Entry.ScalarValue.Reset();
    Entry.ArrayValues.Reset();

    Entry.ArrayValues.Reserve(Values.Num());
    for (float V : Values)
    {
        Entry.ArrayValues.Add(LexToString(V));
    }
}

void UMotaEzSaveData::SetBoolArray(const FString& Key, const TArray<bool>& Values)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Bool;
    Entry.ScalarValue.Reset();
    Entry.ArrayValues.Reset();

    Entry.ArrayValues.Reserve(Values.Num());
    for (bool V : Values)
    {
        Entry.ArrayValues.Add(BoolToString(V));
    }
}

void UMotaEzSaveData::SetStringArray(const FString& Key, const TArray<FString>& Values)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::String;
    Entry.ScalarValue.Reset();
    Entry.ArrayValues = Values;
}

void UMotaEzSaveData::SetVectorArray(const FString& Key, const TArray<FVector>& Values)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Vector;
    Entry.ScalarValue.Reset();
    Entry.ArrayValues.Reset();

    Entry.ArrayValues.Reserve(Values.Num());
    for (const FVector& V : Values)
    {
        Entry.ArrayValues.Add(VectorToString(V));
    }
}

void UMotaEzSaveData::SetRotatorArray(const FString& Key, const TArray<FRotator>& Values)
{
    FMotaEzValue& Entry = Data.FindOrAdd(Key);
    Entry.Type = EMotaEzFieldType::Rotator;
    Entry.ScalarValue.Reset();
    Entry.ArrayValues.Reset();

    Entry.ArrayValues.Reserve(Values.Num());
    for (const FRotator& R : Values)
    {
        Entry.ArrayValues.Add(RotatorToString(R));
    }
}
