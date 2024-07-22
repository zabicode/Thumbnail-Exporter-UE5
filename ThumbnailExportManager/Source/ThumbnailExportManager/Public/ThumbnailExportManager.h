// Copyright (c) 2024, zabicode. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "Modules/ModuleManager.h"

UENUM(BlueprintType)
enum class EThumbnailExportFormat : uint8
{
	PNG UMETA(DisplayName = ".png"),
	JPEG UMETA(DisplayName = ".jpeg"),
	BMP UMETA(DisplayName = ".bmp")
};

UENUM(BlueprintType)
enum class EThumbnailExportResolution : uint8
{
	LOW UMETA(DisplayName = "128x128"),
	MEDIUM UMETA(DisplayName = "256x256"),
	HIGH UMETA(DisplayName = "512x512"),
	MEGA UMETA(DisplayName = "1024x1024")
};

struct FThumbnailResolution2
{
	int32 ImageWidth;
	int32 ImageHeight;

	FThumbnailResolution2()
		: ImageWidth(256), ImageHeight(256) {}

	FThumbnailResolution2(int32 InWidth, int32 InHeight)
		: ImageWidth(InWidth), ImageHeight(InHeight) {}
};



class FThumbnailExportManagerModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FString RemoveAllPrefix(FString Path);

	EThumbnailExportFormat SelectedThumbnailExportFormat = EThumbnailExportFormat::PNG;
	EThumbnailExportResolution SelectedThumbnailExportResolution = EThumbnailExportResolution::MEDIUM;

	FThumbnailResolution2 ThumbnailResolution;
	
	FString SelectedFileExstension = "png";
	FString ExportFolderPath;


	void SetExportFormat(FString ExportFormatString);
	void SetExportResolution(FString ExportResolutionString);
	void SetExportPath(FString ExportPathString);


#pragma region ThumbnailExport

	UFUNCTION(CallInEditor)
	void ExportAssetThumbnail();

#pragma endregion


private:

#pragma region ContentBrowserMenuExtension
	void InitCBMenuExtension();

	TSharedRef<FExtender> CustomCBMenuPathExtender(const TArray<FString>& SelectedPaths);
	TSharedRef<FExtender> CustomCBMenuAssetExtender(const TArray<FAssetData>& SelectedAssets);

	void AddCBMenuPathEntry(FMenuBuilder& MenuBuilder);
	void AddCBMenuAssetEntry(FMenuBuilder& MenuBuilder);

	void OnExportSelectedAssetThumbnailButtonClicked();

#pragma endregion

#pragma region CustomEditorTab

	void RegisterExportThumbnailTab();

	TSharedRef<SDockTab> OnSpawnExportThumbnailTab(const FSpawnTabArgs& SpawnTabArgs);

#pragma endregion


};
