// Copyright (c) 2024, zabicode. All rights reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"


class SExportThumbnailTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SExportThumbnailTab) {}

	SLATE_ARGUMENT(FString, TestInputString)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

	FText GetDestinationFolderPathText() const;

	void RefreshDestinationFolderPathDisplay(const FString& NewDestinationFolderPath);

	TAttribute<FText> DestinationFolderPathValueAttr;

	FString OpenFileDialog();

private:

	FString DestinationFolderPathValue = "No folder selected" ;

	TArray< TSharedPtr <FString> > ComboBoxThumbnailFormatSourceItems;
	TArray< TSharedPtr <FString> > ComboBoxThumbnailResolutionSourceItems;

	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> SourceItem);

	void OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);
	void OnComboSelectionChanged2(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);

	TSharedPtr<STextBlock> ComboDiplayTextBlockFormat;
	TSharedPtr<STextBlock> ComboDiplayTextBlockResolution;

	FReply OnSelectFolderButtonClicked();
	FReply OnExportThumbnailsClicked();
};