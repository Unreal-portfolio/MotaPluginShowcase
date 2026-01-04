#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "MotaEzTypes.h"

class SMotaEzSchemaEditorWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMotaEzSchemaEditorWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // Refrescar el preview del schema
    void RefreshPreview();

    // Guardar el schema actual
    void SaveSchema();

    // Cargar schema existente
    void LoadSchema();

    // Generar desde USaveGame class
    void GenerateFromSaveGameClass();

    // Agregar un nuevo campo
    void AddField();

    // Eliminar un campo
    void RemoveField(const FString& FieldName);

private:

    // Schema actual que se está editando
    FMotaEzSaveSchema CurrentSchema;

    // Widget de lista de campos
    TSharedPtr<class SListView<TSharedPtr<FMotaEzFieldDescriptor>>> FieldListView;

    // Array de campos para la lista
    TArray<TSharedPtr<FMotaEzFieldDescriptor>> FieldsArray;

    // Preview text del schema
    TSharedPtr<class SMultiLineEditableTextBox> PreviewTextBox;

    // Callback cuando se hace clic en guardar
    FReply OnSaveButtonClicked();

    // Callback cuando se hace clic en cargar
    FReply OnLoadButtonClicked();

    // Callback cuando se hace clic en generar desde clase
    FReply OnGenerateFromClassButtonClicked();

    // Callback cuando se hace clic en agregar campo
    FReply OnAddFieldButtonClicked();

    // Generar una row para un campo en la lista
    TSharedRef<class ITableRow> OnGenerateFieldRow(TSharedPtr<FMotaEzFieldDescriptor> Item, const TSharedRef<class STableViewBase>& OwnerTable);

    // Actualizar el array de campos desde el schema
    void RebuildFieldsArray();

    // Crear el preview text desde el schema actual
    FString GeneratePreviewText() const;
};

