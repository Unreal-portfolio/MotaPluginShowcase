#include "SaveComponent.h"
#include "MotaEzSlotManager.h"
#include "MotaEzSaveGameLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

USaveComponent::USaveComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

static void ShowOnScreen(UWorld* World, const FString& Msg, const FColor& Color = FColor::Green, float Time = 4.0f)
{
    if (World && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Time, Color, Msg);
    }
}

void USaveComponent::SaveToSlot(const FString& SlotName)
{
    if (!GetOwner()) return;

    // Determinar el ID real que vamos a usar
    const FString ActualId = !SaveId.IsEmpty() ? SaveId : GetOwner()->GetName();
    
    ShowOnScreen(GetWorld(), FString::Printf(TEXT("[Save] Saving '%s' to '%s'"), *ActualId, *SlotName), FColor::Cyan);

    // Obtener posición y rotación actuales
    const FVector Loc = GetOwner()->GetActorLocation();
    const FRotator Rot = GetOwner()->GetActorRotation();
    
    // Keys simples SIN prefijos de slot (el sistema de slots ya se encarga de la separación)
    const FString PosKey = ActualId + TEXT("_Pos");
    const FString RotKey = ActualId + TEXT("_Rot");
    
    // PASO 1: Escribir datos a CurrentSave (memoria)
    UMotaEzSaveGameLibrary::EzSetVector(GetOwner(), PosKey, Loc);
    UMotaEzSaveGameLibrary::EzSetRotator(GetOwner(), RotKey, Rot);
    
    ShowOnScreen(GetWorld(), FString::Printf(TEXT("Data: Pos=%s Rot=%s"), *Loc.ToString(), *Rot.ToString()), FColor::Yellow);

    // PASO 2: Persistir CurrentSave al archivo del slot correspondiente
    FString Error;
    bool bSuccess = false;

    if (SlotName.Equals(TEXT("ManualSlot"), ESearchCase::IgnoreCase))
    {
        // Slot 0 para guardado manual
        bSuccess = UMotaEzSlotManager::SaveToSlot(GetOwner(), 0, Error);
    }
    else if (SlotName.Equals(QuickSlotName, ESearchCase::IgnoreCase))
    {
        // QuickSave usa el último slot usado (o crea uno nuevo)
        bSuccess = UMotaEzSlotManager::QuickSaveToSlot(GetOwner(), Error);
    }

    if (bSuccess)
    {
        ShowOnScreen(GetWorld(), FString::Printf(TEXT("[Save] SUCCESS: %s"), *SlotName), FColor::Green);
        UE_LOG(LogTemp, Display, TEXT("[SaveComponent] Saved successfully to %s"), *SlotName);
    }
    else
    {
        ShowOnScreen(GetWorld(), FString::Printf(TEXT("[Save] FAILED: %s"), *Error), FColor::Red);
        UE_LOG(LogTemp, Error, TEXT("[SaveComponent] Save failed: %s"), *Error);
    }
}

void USaveComponent::LoadFromSlot(const FString& SlotName)
{
    if (!GetOwner()) return;

    // Determinar el ID real
    const FString ActualId = !SaveId.IsEmpty() ? SaveId : GetOwner()->GetName();
    
    ShowOnScreen(GetWorld(), FString::Printf(TEXT("[Load] Loading '%s' from '%s'"), *ActualId, *SlotName), FColor::Cyan);

    // PASO 1: Cargar el archivo del slot en CurrentSave (memoria)
    FString Error;
    bool bSuccess = false;

    if (SlotName.Equals(TEXT("ManualSlot"), ESearchCase::IgnoreCase))
    {
        bSuccess = UMotaEzSlotManager::LoadFromSlot(GetOwner(), 0, Error);
    }
    else if (SlotName.Equals(QuickSlotName, ESearchCase::IgnoreCase))
    {
        bSuccess = UMotaEzSlotManager::QuickLoadFromSlot(GetOwner(), Error);
    }

    if (!bSuccess)
    {
        ShowOnScreen(GetWorld(), FString::Printf(TEXT("[Load] FAILED: %s"), *Error), FColor::Red);
        UE_LOG(LogTemp, Warning, TEXT("[SaveComponent] Load failed: %s"), *Error);
        return;
    }

    // PASO 2: Leer los datos desde CurrentSave (ahora contiene los datos del archivo cargado)
    const FString PosKey = ActualId + TEXT("_Pos");
    const FString RotKey = ActualId + TEXT("_Rot");
    
    const FVector Loc = UMotaEzSaveGameLibrary::EzGetVector(GetOwner(), PosKey, FVector::ZeroVector);
    const FRotator Rot = UMotaEzSaveGameLibrary::EzGetRotator(GetOwner(), RotKey, FRotator::ZeroRotator);
    
    ShowOnScreen(GetWorld(), FString::Printf(TEXT("Loaded: Pos=%s Rot=%s"), *Loc.ToString(), *Rot.ToString()), FColor::Yellow);
    
    // PASO 3: Aplicar la transformación al actor
    if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
    {
        // Para personajes, detener movimiento primero
        if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
        {
            MoveComp->StopMovementImmediately();
        }
        
        // Aplicar rotación al actor
        const bool bTeleported = Char->TeleportTo(Loc, Rot, false, true);
        
        // IMPORTANTE: Aplicar también la rotación al controller (cámara/control)
        if (AController* Controller = Char->GetController())
        {
            Controller->SetControlRotation(Rot);
            UE_LOG(LogTemp, Display, TEXT("[SaveComponent] Set controller rotation to: %s"), *Rot.ToString());
        }
        
        ShowOnScreen(GetWorld(), FString::Printf(TEXT("[Load] Teleported: Pos=%s Rot=%s"), *Loc.ToString(), *Rot.ToString()), 
            bTeleported ? FColor::Green : FColor::Red);
        UE_LOG(LogTemp, Display, TEXT("[SaveComponent] Character teleported to Loc=%s Rot=%s (Result=%d)"), 
            *Loc.ToString(), *Rot.ToString(), bTeleported);
    }
    else
    {
        // Para actores normales
        GetOwner()->SetActorLocationAndRotation(Loc, Rot, false, nullptr, ETeleportType::TeleportPhysics);
        ShowOnScreen(GetWorld(), TEXT("[Load] Actor position & rotation SET"), FColor::Green);
        UE_LOG(LogTemp, Display, TEXT("[SaveComponent] Actor set to Loc=%s Rot=%s"), *Loc.ToString(), *Rot.ToString());
    }
}

void USaveComponent::QuickSave()
{
    SaveToSlot(QuickSlotName);
}

void USaveComponent::QuickLoad()
{
    LoadFromSlot(QuickSlotName);
}
