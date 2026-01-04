#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "SMotaEzSchemaEditorWidget.h"
#include "PropertyEditorModule.h"
#include "MotaEzSaveGameSettingsCustomization.h"
#include "MotaEzSaveGameSettings.h"

static const FName EzSaveGameSchemaEditorTabName("EzSaveGameSchemaEditor");

class FEzSaveGameEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("EzSaveGameEditor: Startup"));

        // Register property customization for Settings
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.RegisterCustomClassLayout(
            UMotaEzSaveGameSettings::StaticClass()->GetFName(),
            FOnGetDetailCustomizationInstance::CreateStatic(&FMotaEzSaveGameSettingsCustomization::MakeInstance)
        );

        // Registrar tab spawner para el Schema Editor
        FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
            EzSaveGameSchemaEditorTabName,
            FOnSpawnTab::CreateRaw(this, &FEzSaveGameEditorModule::OnSpawnSchemaEditorTab))
            .SetDisplayName(FText::FromString(TEXT("EzSaveGame Schema Editor")))
            .SetMenuType(ETabSpawnerMenuType::Hidden)
            .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());

        // Agregar entrada de menú
        if (FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>("LevelEditor"))
        {
            TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
            MenuExtender->AddMenuExtension(
                "WindowLayout",
                EExtensionHook::After,
                nullptr,
                FMenuExtensionDelegate::CreateRaw(this, &FEzSaveGameEditorModule::AddMenuExtension));

            LevelEditorModule->GetMenuExtensibilityManager()->AddExtender(MenuExtender);
        }

        UE_LOG(LogTemp, Log, TEXT("EzSaveGameEditor: Schema Editor registrado en Window menu"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("EzSaveGameEditor: Shutdown"));

        // Unregister property customization
        if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
            PropertyModule.UnregisterCustomClassLayout(UMotaEzSaveGameSettings::StaticClass()->GetFName());
        }

        // Unregister tab
        FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EzSaveGameSchemaEditorTabName);
    }

private:

    TSharedRef<SDockTab> OnSpawnSchemaEditorTab(const FSpawnTabArgs& Args)
    {
        return SNew(SDockTab)
            .TabRole(ETabRole::NomadTab)
            [
                SNew(SMotaEzSchemaEditorWidget)
            ];
    }

    void AddMenuExtension(FMenuBuilder& Builder)
    {
        Builder.AddMenuEntry(
            FText::FromString(TEXT("EzSaveGame Schema Editor")),
            FText::FromString(TEXT("Abre el editor visual de schemas de EzSaveGame")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FEzSaveGameEditorModule::OpenSchemaEditor))
        );
    }

    void OpenSchemaEditor()
    {
        FGlobalTabmanager::Get()->TryInvokeTab(EzSaveGameSchemaEditorTabName);
    }
};

IMPLEMENT_MODULE(FEzSaveGameEditorModule, EzSaveGameEditor);
