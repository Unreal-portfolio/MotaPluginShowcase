#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MOTAPLUGINSHOWCASE_API USaveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USaveComponent();

    UFUNCTION(BlueprintCallable, Category="Save")
    void SaveToSlot(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category="Save")
    void LoadFromSlot(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category="Save")
    void QuickSave();

    UFUNCTION(BlueprintCallable, Category="Save")
    void QuickLoad();

    // Identificador opcional para el objeto. Si está vacío se usará el nombre del actor propietario.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FString SaveId;

protected:
    FString QuickSlotName = TEXT("QuickSlot");
};
