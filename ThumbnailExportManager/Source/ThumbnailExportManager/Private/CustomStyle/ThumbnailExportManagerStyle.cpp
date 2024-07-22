// Copyright (c) 2024, zabicode. All rights reserved.


#include "CustomStyle/ThumbnailExportManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FThumbnailExportManagerStyle::StyleSetName = FName("ThumbnailExportManagerStyle");
TSharedPtr<FSlateStyleSet> FThumbnailExportManagerStyle::CreatedSlateStyleSet = nullptr;

void FThumbnailExportManagerStyle::InitializeIcons()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}



TSharedRef<FSlateStyleSet> FThumbnailExportManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("ThumbnailExportManager"))->GetBaseDir() / "Resources";

	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon16x16(16.f, 16.f);

	CustomStyleSet->Set("ContentBrowser.ExportThumbnails", new FSlateImageBrush(IconDirectory/"export thumbnails_1.png", Icon16x16));

	return CustomStyleSet;
}

void FThumbnailExportManagerStyle::ShutDown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}
