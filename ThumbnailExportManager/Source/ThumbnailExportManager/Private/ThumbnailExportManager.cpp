// Copyright (c) 2024, zabicode. All rights reserved.

#include "ThumbnailExportManager.h"
#include "DebugHeader.h"
#include "ContentBrowserModule.h"

#include "IContentBrowserSingleton.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h" // Für FAssetRegistryModule
#include "AssetRegistry/AssetData.h"           // Für FAssetData
#include "ObjectTools.h"         // Thumbnail Tools
#include "IImageWrapper.h"       // Für IImageWrapper
#include "IImageWrapperModule.h" // Für IImageWrapperModule
#include "Modules/ModuleManager.h" // Für FModuleManager
#include "Misc/FileHelper.h"     // Für FFileHelper
#include "ImageCore.h"
#include "Misc/Paths.h"

#include "DesktopPlatformModule.h" // Für FileOpener
#include "SlateWidgets/ExportThumbnailWidget.h"

#include "CustomStyle/ThumbnailExportManagerStyle.h"



#define LOCTEXT_NAMESPACE "FThumbnailExportManagerModule"

void FThumbnailExportManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    FThumbnailExportManagerStyle::InitializeIcons();

	InitCBMenuExtension();
    RegisterExportThumbnailTab();
}

#pragma region ContentBrowserMenuExtension

void FThumbnailExportManagerModule::InitCBMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule =
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& PathMenuExtenders = 
	ContentBrowserModule.GetAllPathViewContextMenuExtenders();  // Get content browser menu extenders -> for function call on selected folders in content browser 

	TArray<FContentBrowserMenuExtender_SelectedAssets>& AssetMenuExtenders =
	ContentBrowserModule.GetAllAssetViewContextMenuExtenders(); // Get asset menu extenders -> for function call on selected assets

	PathMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::
		CreateRaw(this, &FThumbnailExportManagerModule::CustomCBMenuPathExtender)); // FIRST BINDING CB

	AssetMenuExtenders.Add(FContentBrowserMenuExtender_SelectedAssets::
		CreateRaw(this, &FThumbnailExportManagerModule::CustomCBMenuAssetExtender));
}

TSharedRef<FExtender> FThumbnailExportManagerModule::CustomCBMenuPathExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FThumbnailExportManagerModule::AddCBMenuPathEntry));  // SECOND BINDING
	}

	return MenuExtender;
}

TSharedRef<FExtender> FThumbnailExportManagerModule::CustomCBMenuAssetExtender(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedAssets.Num() > 0)
	{
		MenuExtender->AddMenuExtension(FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FThumbnailExportManagerModule::AddCBMenuAssetEntry));  // SECOND BINDING
	}

	return MenuExtender;
}

void FThumbnailExportManagerModule::AddCBMenuPathEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Export Asset Thumbnails")),
		FText::FromString(TEXT("Exports all asset thumbnails from assets inside a selected folder structure to an external destination folder")),
		FSlateIcon(FThumbnailExportManagerStyle::GetStyleSetName(), "ContentBrowser.ExportThumbnails"),
		FExecuteAction::CreateRaw(this, &FThumbnailExportManagerModule::OnExportSelectedAssetThumbnailButtonClicked));  // THIRD BINDING
}

void FThumbnailExportManagerModule::AddCBMenuAssetEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Export Asset Thumbnails")),
		FText::FromString(TEXT("Exports asset thumbnails from selected assets to an external destination folder")),
        FSlateIcon(FThumbnailExportManagerStyle::GetStyleSetName(), "ContentBrowser.ExportThumbnails"),
		FExecuteAction::CreateRaw(this, &FThumbnailExportManagerModule::OnExportSelectedAssetThumbnailButtonClicked));
}

void FThumbnailExportManagerModule::OnExportSelectedAssetThumbnailButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(FName("ExportThumbnail"));
}

#pragma endregion


#pragma region CustomEditorTab


void FThumbnailExportManagerModule::RegisterExportThumbnailTab()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("ExportThumbnail"),
    FOnSpawnTab::CreateRaw(this,&FThumbnailExportManagerModule::OnSpawnExportThumbnailTab))
    .SetDisplayName(FText::FromString(TEXT("Export Thumbnail")))
    .SetIcon(FSlateIcon(FThumbnailExportManagerStyle::GetStyleSetName(), "ContentBrowser.ExportThumbnails"));
}

TSharedRef<SDockTab> FThumbnailExportManagerModule::OnSpawnExportThumbnailTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return
    SNew(SDockTab).TabRole(ETabRole::NomadTab)
    [
        SNew(SExportThumbnailTab)
    ];
}

#pragma endregion


void FThumbnailExportManagerModule::SetExportFormat(FString ExportFormatString)
{
    if (ExportFormatString == ".png")
    {
        SelectedThumbnailExportFormat = EThumbnailExportFormat::PNG;
    }
    else if (ExportFormatString == ".jpeg")
    {
        SelectedThumbnailExportFormat = EThumbnailExportFormat::JPEG;
    }
    else if (ExportFormatString == ".bmp")
    {
        SelectedThumbnailExportFormat = EThumbnailExportFormat::BMP;
    }
    else
    {
        DebugHeader::Print(TEXT("Unsupported format"), FColor::Red);
        return;
    }
}

void FThumbnailExportManagerModule::SetExportResolution(FString ExportResolutionString)
{
    if (ExportResolutionString == "128x128")
    {
        SelectedThumbnailExportResolution = EThumbnailExportResolution::LOW;
    }
    else if (ExportResolutionString == "256x256")
    {
        SelectedThumbnailExportResolution = EThumbnailExportResolution::MEDIUM;
    }
    else if (ExportResolutionString == "512x512")
    {
        SelectedThumbnailExportResolution = EThumbnailExportResolution::HIGH;
    }
    else if (ExportResolutionString == "1024x1024")
    {
        SelectedThumbnailExportResolution = EThumbnailExportResolution::MEGA;
    }
    else
    {
        DebugHeader::Print(TEXT("Unsupported format"), FColor::Red);
        return;
    }
}

void FThumbnailExportManagerModule::SetExportPath(FString ExportPathString)
{
    if (!ExportPathString.IsEmpty())
    {
        ExportFolderPath = ExportPathString;
    }
    else
    {
        DebugHeader::Print(TEXT("Export path not found"), FColor::Red);
    }
}


#pragma region ThumbnailExport


FString FThumbnailExportManagerModule::RemoveAllPrefix( FString Path)
{
    FString PrefixToRemove = TEXT("/All");

    FString Result = Path;
    if (Result.RemoveFromStart(PrefixToRemove))
    {
        DebugHeader::Print(TEXT("Path after removing prefix: ") + Result, FColor::Green);
    }
    else
    {
        DebugHeader::Print(TEXT("Path does not start with the prefix"), FColor::Red);
    }

    return Result;
}

void FThumbnailExportManagerModule::ExportAssetThumbnail()
{
    EThumbnailExportFormat EThumbnailFormat = SelectedThumbnailExportFormat;
    EThumbnailExportResolution EThumbnailResolution = SelectedThumbnailExportResolution;
    FThumbnailResolution2 FImageResolution = ThumbnailResolution;
    FString FileExstension = SelectedFileExstension;
    FString ExportPath = ExportFolderPath;

    EImageFormat ImageFormat;


    switch (EThumbnailFormat)
    {
    case EThumbnailExportFormat::PNG:
        ImageFormat = EImageFormat::PNG;
        FileExstension = "png";
        break;
    case EThumbnailExportFormat::JPEG:
        ImageFormat = EImageFormat::JPEG;
        FileExstension = "jpeg";
        break;
    case EThumbnailExportFormat::BMP:
        ImageFormat = EImageFormat::BMP;
        FileExstension = "bmp";
        break;
    default:
        DebugHeader::Print(TEXT("Unsupported format"), FColor::Red);
        return;
    }

    switch (EThumbnailResolution)
    {
    case EThumbnailExportResolution::LOW:
        FImageResolution.ImageHeight = 128;
        FImageResolution.ImageWidth = 128;
    
        break;
    case EThumbnailExportResolution::MEDIUM:
        FImageResolution.ImageHeight = 256;
        FImageResolution.ImageWidth = 256;
     
        break;
    case EThumbnailExportResolution::HIGH:
        FImageResolution.ImageHeight = 512;
        FImageResolution.ImageWidth = 512;
     
        break;
    case EThumbnailExportResolution::MEGA:
        FImageResolution.ImageHeight = 1024;
        FImageResolution.ImageWidth = 1024;
       
        break;
    default:
        DebugHeader::Print(TEXT("Unsupported resolution"), FColor::Red);
        return;
    }

    // Get selected assets and selected paths
    TArray<FAssetData> SelectedAssetsData;
    TArray<FString> SelectedPaths = UEditorUtilityLibrary::GetSelectedFolderPaths();

    if (SelectedPaths.Num() > 1)
    {
        DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"),false);
        return;
    }

    if (SelectedPaths.Num() == 0)
    {
        SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

    }

    else
    {
        FString PrefixfreeSelectedPath = RemoveAllPrefix(SelectedPaths[0]);

        TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(PrefixfreeSelectedPath);

        if (AssetsPathNames.Num() == 0)
        {
            DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset found under selected folder"), false);
            return;
        }

        EAppReturnType::Type ConfirmResult =
            DebugHeader::ShowMsgDialog(EAppMsgType::YesNo, TEXT("A total of ") + FString::FromInt(AssetsPathNames.Num()) + TEXT(" found.\nWoudle you like to procceed?"), false);

        if (ConfirmResult == EAppReturnType::No) return;

        for (const FString& AssetPathName : AssetsPathNames)
        {
            if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName)) continue;

            SelectedAssetsData.Add(UEditorAssetLibrary::FindAssetData(AssetPathName));
        }
    }

   
    IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        int RetryCount = 50;
        bool bSuccess = false;

        while (RetryCount-- > 0)
        {
            if (!SelectedAssetData.IsValid())
            {
                DebugHeader::Print(TEXT("Asset data not valid"), FColor::Red);
                break;
            }

            UObject* Asset = SelectedAssetData.GetAsset();

            if (!Asset)
            {
                DebugHeader::Print(FString::Printf(TEXT("Failed to fetch Asset: %s"), *SelectedAssetData.AssetName.ToString()), FColor::Red);
                break;  
            }

            // Generate thumbnail for the asset
            FObjectThumbnail* ObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);
            if (!ObjectThumbnail)
            {
                DebugHeader::Print(TEXT("Failed to fetch ObjectThumbnail"), FColor::Red);
                break; 
            }

            // Create image wrapper for the thumbnail
            TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
            if (!ImageWrapper)
            {
                DebugHeader::Print(TEXT("Failed to create ImageWrapper"), FColor::Red);
                break;  
            }

            // Zugriff auf die Rohdaten des Thumbnails
            TArray<uint8> ThumbnailData = ObjectThumbnail->GetUncompressedImageData();
            if (ThumbnailData.IsEmpty())
            {
                break;
            }

            int32 ImageWidth = ObjectThumbnail->GetImageWidth();
            if (ImageWidth == 0)
            {
                break;
            }

            int32 ImageHeight = ObjectThumbnail->GetImageHeight();
            if (ImageHeight == 0)
            {
                break;
            }

            // Konvertierung der Rohdaten zu FColor
            int32 NumPixels = ImageWidth * ImageHeight;
            if (NumPixels == 0)
            {
                break;
            }

            TArray<FColor> ColorData;

            ColorData.SetNumUninitialized(NumPixels);
            if (ColorData.IsEmpty())
            {
                break;
            }

            // Kopieren der Rohdaten in das FColor-Array
            FMemory::Memcpy(ColorData.GetData(), ThumbnailData.GetData(), NumPixels * sizeof(FColor));

            // Erstellen eines FImageView für das Quellbild
            FImageView SourceView(ColorData.GetData(), ImageWidth, ImageHeight, ERawImageFormat::BGRA8);
            if (!SourceView.IsImageInfoValid())
            {
                break;
            }

            // Zielbild-Array erstellen
            int32 TargetImageWidth = FImageResolution.ImageWidth;
            if (TargetImageWidth == 0)
            {
                break;
            }

            int32 TargetImageHeight = FImageResolution.ImageHeight;
            if (TargetImageHeight == 0)
            {
                break;
            }

            int32 TargetNumPixels = TargetImageWidth * TargetImageHeight;
            if (TargetNumPixels == 0)
            {
                break;
            }

            TArray<FColor> TargetColorData;

            TargetColorData.SetNumUninitialized(TargetNumPixels);

            if (TargetColorData.IsEmpty())
            {
                break;
            }

            // Erstellen eines FImageView für das Zielbild
            FImageView TargetView(TargetColorData.GetData(), TargetImageWidth, TargetImageHeight, ERawImageFormat::BGRA8);
            if (!TargetView.IsImageInfoValid())
            {
                break;
            }

            // Skalieren des Bildes
            FImageCore::ResizeImage(SourceView, TargetView);


            // Set the raw data for the image wrapper
            ImageWrapper->SetRaw(TargetColorData.GetData(), TargetNumPixels * sizeof(FColor), TargetImageWidth, TargetImageHeight, ERGBFormat::BGRA, 8);

            // Compress the thumbnail to the desired format
            const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();
            if (CompressedByteArray.Num() == 0)
            {
                DebugHeader::Print(TEXT("Failed to compress thumbnail"), FColor::Red);

                break;
            }

            // Generate a unique file name for each asset
            FString AssetName = Asset->GetName();
            if (AssetName.IsEmpty())
            {
                break;
            }

            FString FileName = ExportPath / FString::Printf(TEXT("%s.%s"), *AssetName, *FileExstension);
            if (FileName.IsEmpty())
            {
                break;
            }

            // Save the compressed thumbnail to disk
            if (!FFileHelper::SaveArrayToFile(CompressedByteArray, *FileName))
            {
                DebugHeader::Print(FString::Printf(TEXT("Failed to save thumbnail for %s"), *AssetName), FColor::Red);
            }
            else
            {
                DebugHeader::Print(FString::Printf(TEXT("Thumbnail saved for %s"), *AssetName), FColor::Green);
            }

            // If we reach this point, everything was successful
            bSuccess = true;
            break; 

        }

        if (!bSuccess)
        {
            DebugHeader::Print(FString::Printf(TEXT("Failed to process asset %s after multiple attempts"), *SelectedAssetData.AssetName.ToString()), FColor::Red);
        }
    }
}

#pragma endregion



void FThumbnailExportManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.


    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("ExportThumbnail"));

    FThumbnailExportManagerStyle::ShutDown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FThumbnailExportManagerModule, ThumbnailExportManager)