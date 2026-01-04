/*
 * EzSaveGame Plugin - usage requires attribution to Jose Antonio Mota Lucas per the LICENSE file.
 */
#include "MotaEzSaveGameSettingsCustomization.h"
#include "MotaEzSaveGameSettings.h"
#include "MotaEzSlotManager.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformFileManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "EzSaveGameSettings"

TSharedRef<IDetailCustomization> FMotaEzSaveGameSettingsCustomization::MakeInstance()
{
    return MakeShareable(new FMotaEzSaveGameSettingsCustomization);
}

void FMotaEzSaveGameSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    // Add a new category for utility actions
    IDetailCategoryBuilder& UtilityCategory = DetailBuilder.EditCategory("Utilities", 
        FText::FromString(TEXT("Utilities")), ECategoryPriority::Important);

    // Add "Open Save Folder" button
    UtilityCategory.AddCustomRow(FText::FromString(TEXT("Open Save Folder")))
        .WholeRowContent()
        [
            SNew(SBox)
            .Padding(FMargin(0, 5, 0, 5))
            [
                SNew(SButton)
                .HAlign(HAlign_Center)
                .Text(FText::FromString(TEXT("Open Save Folder")))
                .ToolTipText(FText::FromString(TEXT("Opens the save game folder in Windows Explorer")))
                .OnClicked(this, &FMotaEzSaveGameSettingsCustomization::OnOpenSaveFolderClicked)
            ]
        ];

    // Add "Delete All Save Data" button
    UtilityCategory.AddCustomRow(FText::FromString(TEXT("Delete All Save Data")))
        .WholeRowContent()
        [
            SNew(SBox)
            .Padding(FMargin(0, 5, 0, 5))
            [
                SNew(SButton)
                .HAlign(HAlign_Center)
                .Text(FText::FromString(TEXT("Delete All Save Data")))
                .ToolTipText(FText::FromString(TEXT("WARNING: Deletes ALL save files, slots, and metadata. This action cannot be undone!")))
                .OnClicked(this, &FMotaEzSaveGameSettingsCustomization::OnDeleteAllSaveDataClicked)
            ]
        ];

    // Add warning text
    UtilityCategory.AddCustomRow(FText::FromString(TEXT("Warning")))
        .WholeRowContent()
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("⚠ Delete All Save Data will permanently remove all save files and cannot be undone!")))
            .ColorAndOpacity(FLinearColor(1.0f, 0.5f, 0.0f))
            .AutoWrapText(true)
        ];
}

FReply FMotaEzSaveGameSettingsCustomization::OnOpenSaveFolderClicked()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to get settings for OpenSaveFolder"));
        return FReply::Handled();
    }

    // Get the save directory path
    FString SaveDir = FPaths::Combine(FPaths::ProjectSavedDir(), Settings->SaveDirectory.Path);
    
    // Normalize the path (convert to platform-specific format)
    FPaths::NormalizeDirectoryName(SaveDir);
    SaveDir = FPaths::ConvertRelativePathToFull(SaveDir);
    
    // Create directory if it doesn't exist
    if (!FPaths::DirectoryExists(SaveDir))
    {
        if (IFileManager::Get().MakeDirectory(*SaveDir, true))
        {
            UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Created save directory: %s"), *SaveDir);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("EzSaveGame: Failed to create save directory: %s"), *SaveDir);
            return FReply::Handled();
        }
    }
    
    // Open the folder in explorer
    UE_LOG(LogTemp, Display, TEXT("EzSaveGame: Opening save folder: %s"), *SaveDir);
    FPlatformProcess::ExploreFolder(*SaveDir);
    
    return FReply::Handled();
}

FReply FMotaEzSaveGameSettingsCustomization::OnDeleteAllSaveDataClicked()
{
    // Show confirmation dialog
    EAppReturnType::Type Result = FMessageDialog::Open(
        EAppMsgType::YesNo,
        FText::FromString(TEXT("Are you sure you want to delete ALL save data?\n\n"
                              "This will permanently remove:\n"
                              "• All save slots (Slot0, Slot1, etc.)\n"
                              "• Auto-save data\n"
                              "• Metadata and screenshots\n"
                              "• Configuration files\n"
                              "• Schema files\n\n"
                              "This action CANNOT be undone!")),
        FText::FromString(TEXT("Delete All Save Data - Confirmation"))
    );

    if (Result == EAppReturnType::Yes)
    {
        bool bSuccess = UMotaEzSlotManager::DeleteAllSaveData(20); // Delete up to 20 slots
        
        if (bSuccess)
        {
            FMessageDialog::Open(
                EAppMsgType::Ok,
                FText::FromString(TEXT("All save data has been successfully deleted.")),
                FText::FromString(TEXT("Delete Complete"))
            );
        }
        else
        {
            FMessageDialog::Open(
                EAppMsgType::Ok,
                FText::FromString(TEXT("Some files could not be deleted. Check the Output Log for details.")),
                FText::FromString(TEXT("Delete Incomplete"))
            );
        }
    }
    
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

