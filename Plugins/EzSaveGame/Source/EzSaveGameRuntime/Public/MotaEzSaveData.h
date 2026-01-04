#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MotaEzTypes.h"
#include "MotaEzSaveData.generated.h"

/**
 * Universal save game object used by EzSaveGame plugin.
 * Contains a map of Key -> FMotaEzValue for flexible data storage.
 */
UCLASS(BlueprintType)
class EZSAVEGAMERUNTIME_API UMotaEzSaveData : public USaveGame
{
    GENERATED_BODY()

public:

    UPROPERTY(SaveGame)
    TMap<FString, FMotaEzValue> Data;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    int32 GetInt(const FString& Key, int32 DefaultValue = 0) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    float GetFloat(const FString& Key, float DefaultValue = 0.f) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    bool GetBool(const FString& Key, bool DefaultValue = false) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    FString GetString(const FString& Key, const FString& DefaultValue = TEXT("")) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    FVector GetVector(const FString& Key, const FVector& DefaultValue = FVector::ZeroVector) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    FRotator GetRotator(const FString& Key, const FRotator& DefaultValue = FRotator::ZeroRotator) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetInt(const FString& Key, int32 Value);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetFloat(const FString& Key, float Value);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetBool(const FString& Key, bool Value);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetString(const FString& Key, const FString& Value);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetVector(const FString& Key, const FVector& Value);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetRotator(const FString& Key, const FRotator& Value);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    TArray<int32> GetIntArray(const FString& Key) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    TArray<float> GetFloatArray(const FString& Key) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    TArray<bool> GetBoolArray(const FString& Key) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    TArray<FString> GetStringArray(const FString& Key) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    TArray<FVector> GetVectorArray(const FString& Key) const;

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    TArray<FRotator> GetRotatorArray(const FString& Key) const;


    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetIntArray(const FString& Key, const TArray<int32>& Values);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetFloatArray(const FString& Key, const TArray<float>& Values);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetBoolArray(const FString& Key, const TArray<bool>& Values);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetStringArray(const FString& Key, const TArray<FString>& Values);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetVectorArray(const FString& Key, const TArray<FVector>& Values);

    UFUNCTION(BlueprintCallable, Category="EzSaveGame")
    void SetRotatorArray(const FString& Key, const TArray<FRotator>& Values);
};
