// SMotaEzSchemaEditorWidget.cpp

#include "SMotaEzSchemaEditorWidget.h"
#include "MotaEzSchemaGenerator.h"
#include "MotaEzFormatSerializer.h"
#include "MotaEzSaveGameSettings.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "EditorStyleSet.h"
#include "Misc/FileHelper.h"
#include "Editor.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "GameFramework/SaveGame.h"

// Filtro para mostrar solo clases que heredan de USaveGame
class FSaveGameClassFilter : public IClassViewerFilter
{
public:
    virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
    {
        return InClass && InClass->IsChildOf(USaveGame::StaticClass()) && !InClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated);
    }

    virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
    {
        return InUnloadedClassData->IsChildOf(USaveGame::StaticClass());
    }
};

void SMotaEzSchemaEditorWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)

        // Header
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("EzSaveGame - Editor de Schema")))
            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
        ]

        // Toolbar con botones de acción
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f, 5.0f)
        [
            SNew(SHorizontalBox)

            // Botón Cargar
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("Cargar Schema")))
                .ToolTipText(FText::FromString(TEXT("Carga el schema desde disco")))
                .OnClicked(this, &SMotaEzSchemaEditorWidget::OnLoadButtonClicked)
            ]

            // Botón Guardar
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("Guardar Schema")))
                .ToolTipText(FText::FromString(TEXT("Guarda el schema a disco")))
                .OnClicked(this, &SMotaEzSchemaEditorWidget::OnSaveButtonClicked)
            ]

            // Botón Generar desde Clase
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("Generar desde USaveGame")))
                .ToolTipText(FText::FromString(TEXT("Genera el schema automáticamente desde una clase USaveGame")))
                .OnClicked(this, &SMotaEzSchemaEditorWidget::OnGenerateFromClassButtonClicked)
            ]

            // Botón Agregar Campo
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .Text(FText::FromString(TEXT("+ Agregar Campo")))
                .ToolTipText(FText::FromString(TEXT("Agrega un nuevo campo al schema")))
                .OnClicked(this, &SMotaEzSchemaEditorWidget::OnAddFieldButtonClicked)
            ]
        ]

        // Contenido principal (Lista de campos + Preview)
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(10.0f)
        [
            SNew(SHorizontalBox)

            // Lista de campos (izquierda)
            + SHorizontalBox::Slot()
            .FillWidth(0.5f)
            .Padding(0.0f, 0.0f, 5.0f, 0.0f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(5.0f)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Campos del Schema")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                    ]

                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    [
                        SAssignNew(FieldListView, SListView<TSharedPtr<FMotaEzFieldDescriptor>>)
                        .ListItemsSource(&FieldsArray)
                        .OnGenerateRow(this, &SMotaEzSchemaEditorWidget::OnGenerateFieldRow)
                    ]
                ]
            ]

            // Preview (derecha)
            + SHorizontalBox::Slot()
            .FillWidth(0.5f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FLinearColor(0.2f, 0.2f, 0.2f))
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(5.0f)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Preview")))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                    ]

                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    .Padding(5.0f)
                    [
                        SAssignNew(PreviewTextBox, SMultiLineEditableTextBox)
                        .IsReadOnly(true)
                        .Text(FText::FromString(TEXT("El preview del schema aparecerá aquí...")))
                    ]
                ]
            ]
        ]
    ];

    // Cargar schema inicial si existe
    LoadSchema();
}

void SMotaEzSchemaEditorWidget::RefreshPreview()
{
    FString PreviewText = GeneratePreviewText();
    if (PreviewTextBox.IsValid())
    {
        PreviewTextBox->SetText(FText::FromString(PreviewText));
    }
}

void SMotaEzSchemaEditorWidget::SaveSchema()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        return;
    }

    FString SchemaText;
    FString Error;
    
    if (!UMotaEzFormatSerializer::SaveSchema(Settings->DefaultFormat, CurrentSchema, SchemaText, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Error al serializar schema: %s"), *Error);
        return;
    }

    FString FilePath = Settings->GetSchemaFilePath();
    if (FFileHelper::SaveStringToFile(SchemaText, *FilePath))
    {
        UE_LOG(LogTemp, Display, TEXT("✅ Schema guardado en: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Error al guardar schema en: %s"), *FilePath);
    }
}

void SMotaEzSchemaEditorWidget::LoadSchema()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        return;
    }

    FString FilePath = Settings->GetSchemaFilePath();
    FString SchemaText;
    
    if (!FFileHelper::LoadFileToString(SchemaText, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo cargar schema desde: %s"), *FilePath);
        return;
    }

    FString Error;
    if (UMotaEzFormatSerializer::LoadSchema(Settings->DefaultFormat, SchemaText, CurrentSchema, Error))
    {
        RebuildFieldsArray();
        RefreshPreview();
        UE_LOG(LogTemp, Display, TEXT("✅ Schema cargado desde: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Error al parsear schema: %s"), *Error);
    }
}

void SMotaEzSchemaEditorWidget::GenerateFromSaveGameClass()
{
    // Aquí se abrirá un class picker para seleccionar la clase
    // Por ahora, simplemente loguear
    UE_LOG(LogTemp, Log, TEXT("Generar desde clase - Implementación pendiente de class picker"));
}

void SMotaEzSchemaEditorWidget::AddField()
{
    FMotaEzFieldDescriptor NewField;
    NewField.Name = FString::Printf(TEXT("NewField%d"), CurrentSchema.Fields.Num() + 1);
    NewField.Type = EMotaEzFieldType::String;
    NewField.bIsArray = false;
    NewField.DefaultValue = TEXT("");

    CurrentSchema.Fields.Add(NewField.Name, NewField);
    RebuildFieldsArray();
    RefreshPreview();
}

void SMotaEzSchemaEditorWidget::RemoveField(const FString& FieldName)
{
    CurrentSchema.Fields.Remove(FieldName);
    RebuildFieldsArray();
    RefreshPreview();
}

FReply SMotaEzSchemaEditorWidget::OnSaveButtonClicked()
{
    SaveSchema();
    return FReply::Handled();
}

FReply SMotaEzSchemaEditorWidget::OnLoadButtonClicked()
{
    LoadSchema();
    return FReply::Handled();
}

FReply SMotaEzSchemaEditorWidget::OnGenerateFromClassButtonClicked()
{
    GenerateFromSaveGameClass();
    return FReply::Handled();
}

FReply SMotaEzSchemaEditorWidget::OnAddFieldButtonClicked()
{
    AddField();
    return FReply::Handled();
}

TSharedRef<ITableRow> SMotaEzSchemaEditorWidget::OnGenerateFieldRow(TSharedPtr<FMotaEzFieldDescriptor> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<TSharedPtr<FMotaEzFieldDescriptor>>, OwnerTable)
    [
        SNew(SHorizontalBox)

        // Nombre del campo
        + SHorizontalBox::Slot()
        .FillWidth(0.4f)
        .VAlign(VAlign_Center)
        .Padding(5.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(Item->Name))
        ]

        // Tipo
        + SHorizontalBox::Slot()
        .FillWidth(0.2f)
        .VAlign(VAlign_Center)
        .Padding(5.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(
                Item->bIsArray ? TEXT("Array") : TEXT("Scalar")
            ))
        ]

        // Tipo de dato
        + SHorizontalBox::Slot()
        .FillWidth(0.2f)
        .VAlign(VAlign_Center)
        .Padding(5.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString([Item]() -> FString {
                switch (Item->Type)
                {
                    case EMotaEzFieldType::Int: return TEXT("Int");
                    case EMotaEzFieldType::Float: return TEXT("Float");
                    case EMotaEzFieldType::Bool: return TEXT("Bool");
                    case EMotaEzFieldType::String: return TEXT("String");
                    case EMotaEzFieldType::Vector: return TEXT("Vector");
                    case EMotaEzFieldType::Rotator: return TEXT("Rotator");
                    default: return TEXT("Unknown");
                }
            }()))
        ]

        // Botón eliminar
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .Padding(5.0f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("X")))
            .OnClicked_Lambda([this, Item]() -> FReply {
                RemoveField(Item->Name);
                return FReply::Handled();
            })
        ]
    ];
}

void SMotaEzSchemaEditorWidget::RebuildFieldsArray()
{
    FieldsArray.Reset();

    for (auto& Pair : CurrentSchema.Fields)
    {
        FieldsArray.Add(MakeShared<FMotaEzFieldDescriptor>(Pair.Value));
    }

    if (FieldListView.IsValid())
    {
        FieldListView->RequestListRefresh();
    }
}

FString SMotaEzSchemaEditorWidget::GeneratePreviewText() const
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        return TEXT("Error: No se pudo obtener Settings");
    }

    FString PreviewText;
    FString Error;

    if (!UMotaEzFormatSerializer::SaveSchema(Settings->DefaultFormat, CurrentSchema, PreviewText, Error))
    {
        return FString::Printf(TEXT("Error: %s"), *Error);
    }

    return PreviewText;
}

