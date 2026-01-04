/*
 * EzSaveGame Plugin - requires attribution to Jose Antonio Mota Lucas when redistributed.
 * See LICENSE for full terms and third-party notices.
 */

#include "Modules/ModuleManager.h"
#include "MotaEzConfigSubsystem.h"
#include "MotaEzSaveData.h"
#include "MotaEzSaveGameSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"

static FAutoConsoleCommand ConsoleCommand_EzSave(
    TEXT("EzSave.Save"),
    TEXT("Saves all current data to disk"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (GEngine && GEngine->GameViewport)
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            if (World && World->GetGameInstance())
            {
                UMotaEzConfigSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UMotaEzConfigSubsystem>();
                if (Subsystem)
                {
                    if (Subsystem->SaveToDisk())
                    {
                        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Data saved successfully"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to save data"));
                    }
                }
            }
        }
    })
);

static FAutoConsoleCommand ConsoleCommand_EzLoad(
    TEXT("EzSave.Load"),
    TEXT("Reloads all data from disk"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        if (GEngine && GEngine->GameViewport)
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            if (World && World->GetGameInstance())
            {
                UMotaEzConfigSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UMotaEzConfigSubsystem>();
                if (Subsystem)
                {
                    if (Subsystem->ReloadFromDisk())
                    {
                        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Data loaded successfully"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to load data"));
                    }
                }
            }
        }
    })
);

static FAutoConsoleCommand ConsoleCommand_EzGetInt(
    TEXT("EzSave.GetInt"),
    TEXT("Gets an integer value. Usage: EzSave.GetInt <Key>"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (Args.Num() < 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Usage: EzSave.GetInt <Key>"));
            return;
        }

        if (GEngine && GEngine->GameViewport)
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            if (World && World->GetGameInstance())
            {
                UMotaEzConfigSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UMotaEzConfigSubsystem>();
                if (Subsystem)
                {
                    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
                    if (SaveData)
                    {
                        int32 Value = SaveData->GetInt(Args[0], 0);
                        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: %s = %d"), *Args[0], Value);
                    }
                }
            }
        }
    })
);

static FAutoConsoleCommand ConsoleCommand_EzSetInt(
    TEXT("EzSave.SetInt"),
    TEXT("Sets an integer value. Usage: EzSave.SetInt <Key> <Value>"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (Args.Num() < 2)
        {
            UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Usage: EzSave.SetInt <Key> <Value>"));
            return;
        }

        if (GEngine && GEngine->GameViewport)
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            if (World && World->GetGameInstance())
            {
                UMotaEzConfigSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UMotaEzConfigSubsystem>();
                if (Subsystem)
                {
                    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
                    if (SaveData)
                    {
                        int32 Value = FCString::Atoi(*Args[1]);
                        SaveData->SetInt(Args[0], Value);
                        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: %s = %d (saved in memory, use EzSave.Save to persist)"), *Args[0], Value);
                    }
                }
            }
        }
    })
);

static FAutoConsoleCommand ConsoleCommand_EzGetString(
    TEXT("EzSave.GetString"),
    TEXT("Gets a string value. Usage: EzSave.GetString <Key>"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (Args.Num() < 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("EzSaveGame: Usage: EzSave.GetString <Key>"));
            return;
        }

        if (GEngine && GEngine->GameViewport)
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            if (World && World->GetGameInstance())
            {
                UMotaEzConfigSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UMotaEzConfigSubsystem>();
                if (Subsystem)
                {
                    UMotaEzSaveData* SaveData = Subsystem->GetCurrentSave();
                    if (SaveData)
                    {
                        FString Value = SaveData->GetString(Args[0], TEXT(""));
                        UE_LOG(LogTemp, Display, TEXT("EzSaveGame: %s = \"%s\""), *Args[0], *Value);
                    }
                }
            }
        }
    })
);

static FAutoConsoleCommand ConsoleCommand_EzInfo(
    TEXT("EzSave.Info"),
    TEXT("Displays current EzSaveGame configuration information"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
        if (Settings)
        {
            UE_LOG(LogTemp, Display, TEXT("======================================="));
            UE_LOG(LogTemp, Display, TEXT("EzSaveGame - Configuration Info"));
            UE_LOG(LogTemp, Display, TEXT("======================================="));
            
            FString FormatStr;
            switch (Settings->DefaultFormat)
            {
                case EMotaEzConfigFormat::Toml: FormatStr = TEXT("TOML"); break;
                case EMotaEzConfigFormat::Json: FormatStr = TEXT("JSON"); break;
                case EMotaEzConfigFormat::Yaml: FormatStr = TEXT("YAML"); break;
                case EMotaEzConfigFormat::Xml:  FormatStr = TEXT("XML"); break;
                default: FormatStr = TEXT("Unknown"); break;
            }
            
            UE_LOG(LogTemp, Display, TEXT("Format: %s"), *FormatStr);
            UE_LOG(LogTemp, Display, TEXT("Config File: %s"), *Settings->GetConfigFilePath());
            UE_LOG(LogTemp, Display, TEXT("Schema File: %s"), *Settings->GetSchemaFilePath());
            UE_LOG(LogTemp, Display, TEXT("======================================="));
        }
    })
);

class FEzSaveGameRuntimeModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("EzSaveGameRuntime: Module started"));
        UE_LOG(LogTemp, Log, TEXT("EzSaveGame Console Commands available:"));
        UE_LOG(LogTemp, Log, TEXT("   - EzSave.Save       : Save data"));
        UE_LOG(LogTemp, Log, TEXT("   - EzSave.Load       : Load data"));
        UE_LOG(LogTemp, Log, TEXT("   - EzSave.GetInt     : Get Int value"));
        UE_LOG(LogTemp, Log, TEXT("   - EzSave.SetInt     : Set Int value"));
        UE_LOG(LogTemp, Log, TEXT("   - EzSave.GetString  : Get String value"));
        UE_LOG(LogTemp, Log, TEXT("   - EzSave.Info       : Show info"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("EzSaveGameRuntime: Module shutdown"));
    }
};

IMPLEMENT_MODULE(FEzSaveGameRuntimeModule, EzSaveGameRuntime);
