/*
 * EzSaveGame Plugin - usage requires attribution to Jose Antonio Mota Lucas per the LICENSE file.
 */
#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * Customization for UMotaEzSaveGameSettings to add utility buttons in Project Settings.
 */
class FMotaEzSaveGameSettingsCustomization : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
    FReply OnDeleteAllSaveDataClicked();
    FReply OnOpenSaveFolderClicked();
};

