// Copyright (c) 2024, zabicode. All rights reserved.


#include "AssetActions/QuickAssetAction.h"
#include "DebugHeader.h"
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

#include "DesktopPlatformModule.h" // Für FileOpener

void UQuickAssetAction::ExportAssetThumbnail(EThumbnailFormat Format, EThumbnailResolution Resolution)
{
    EThumbnailFormat ThumbnailFormat = Format;
    EThumbnailResolution ThumbnailResolution = Resolution;

    EImageFormat ImageFormat;
    FImageResolution ImageResolution;
    FString FileExstension;

    FString ExportPath = OpenFileDialog();

    if (ExportPath.IsEmpty())
    {
        DebugHeader::Print(TEXT("Please enter a valid destination folder"), FColor::Cyan);
        return;
    }


    //Print(FString::Printf(TEXT("Export path: '%s'"), *ExportPath), FColor::Yellow);

    FString CleanExportPath = ExportPath.TrimStartAndEnd(); // Remove any leading/trailing whitespace

    //Print(FString::Printf(TEXT("Export path: '%s'"), *CleanExportPath), FColor::Yellow);

    /*
    // Ensure the export path exists
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*CleanExportPath))
    {
        PlatformFile.CreateDirectory(*CleanExportPath);
        if (!PlatformFile.DirectoryExists(*CleanExportPath))
        {
            Print(TEXT("Failed to create export directory"), FColor::Red);
            return;
        }
    }

    */


    switch (ThumbnailFormat)
    {
    case EThumbnailFormat::PNG:
        ImageFormat = EImageFormat::PNG;
        FileExstension = "png";
        break;
    case EThumbnailFormat::JPEG:
        ImageFormat = EImageFormat::JPEG;
        FileExstension = "jpeg";
        break;
    case EThumbnailFormat::BMP:
        ImageFormat = EImageFormat::BMP;
        FileExstension = "bmp";
        break;
    default:
        DebugHeader::Print(TEXT("Unsupported format"), FColor::Red);
        return;
    }

    switch (ThumbnailResolution)
    {
    case EThumbnailResolution::LOW:
        ImageResolution.ImageHeight = 128;
        ImageResolution.ImageWidth = 128;
        break;
    case EThumbnailResolution::MEDIUM:
        ImageResolution.ImageHeight = 256;
        ImageResolution.ImageWidth = 256;
        break;
    case EThumbnailResolution::HIGH:
        ImageResolution.ImageHeight = 512;
        ImageResolution.ImageWidth = 512;
        break;
    case EThumbnailResolution::MEGA:
        ImageResolution.ImageHeight = 1024;
        ImageResolution.ImageWidth = 1024;
        break;
    default:
        DebugHeader::Print(TEXT("Unsupported resolution"), FColor::Red);
        return;
    }


    TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
    IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        UObject* Asset = SelectedAssetData.GetAsset();
        if (!Asset)
        {
            DebugHeader::Print(FString::Printf(TEXT("Failed to fetch Asset: %s"), *SelectedAssetData.AssetName.ToString()), FColor::Red);
            continue;
        }

        // Generate thumbnail for the asset
        FObjectThumbnail* ObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);
        if (!ObjectThumbnail)
        {
            DebugHeader::Print(TEXT("Failed to fetch ObjectThumbnail"), FColor::Red);
            continue;
        }

        // Create image wrapper for the thumbnail
        TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
        if (!ImageWrapper)
        {
            DebugHeader::Print(TEXT("Failed to create ImageWrapper"), FColor::Red);
            continue;
        }

        // Zugriff auf die Rohdaten des Thumbnails
        TArray<uint8> ThumbnailData = ObjectThumbnail->GetUncompressedImageData();
        int32 ImageWidth = ObjectThumbnail->GetImageWidth();
        int32 ImageHeight = ObjectThumbnail->GetImageHeight();

        // Konvertierung der Rohdaten zu FColor
        int32 NumPixels = ImageWidth * ImageHeight;
        TArray<FColor> ColorData;
        ColorData.SetNumUninitialized(NumPixels);

        // Kopieren der Rohdaten in das FColor-Array
        FMemory::Memcpy(ColorData.GetData(), ThumbnailData.GetData(), NumPixels * sizeof(FColor));

        // Erstellen eines FImageView für das Quellbild
        FImageView SourceView(ColorData.GetData(), ImageWidth, ImageHeight, ERawImageFormat::BGRA8);

        // Zielbild-Array erstellen
        int32 TargetImageWidth = ImageResolution.ImageWidth;
        int32 TargetImageHeight = ImageResolution.ImageHeight;

        int32 TargetNumPixels = TargetImageWidth * TargetImageHeight;
        TArray<FColor> TargetColorData;
        TargetColorData.SetNumUninitialized(TargetNumPixels);

        // Erstellen eines FImageView für das Zielbild
        FImageView TargetView(TargetColorData.GetData(), TargetImageWidth, TargetImageHeight, ERawImageFormat::BGRA8);

        // Skalieren des Bildes
        FImageCore::ResizeImage(SourceView, TargetView);

     // Set the raw data for the image wrapper
        ImageWrapper->SetRaw(TargetColorData.GetData(), TargetNumPixels * sizeof(FColor), TargetImageWidth, TargetImageHeight, ERGBFormat::BGRA, 8);

        // Compress the thumbnail to the desired format
        const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();
        if (CompressedByteArray.Num() == 0)
        {
            continue;
        }

        // Generate a unique file name for each asset
        FString AssetName = Asset->GetName();
        FString FileName = CleanExportPath / FString::Printf(TEXT("%s.%s"), *AssetName, *FileExstension);

        // Save the compressed thumbnail to disk
        if (!FFileHelper::SaveArrayToFile(CompressedByteArray, *FileName))
        {
            DebugHeader::Print(FString::Printf(TEXT("Failed to save thumbnail for %s"), *AssetName), FColor::Red);
        }
        else
        {
            DebugHeader::Print(FString::Printf(TEXT("Thumbnail saved for %s"), *AssetName), FColor::Green);
        }
    }
}

FString UQuickAssetAction::OpenFileDialog()
{

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        // Define the window title
        const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
        FString FolderPath;
        const FString Title = "Choose a directory";
        const FString DefaultPath = FPaths::ProjectDir();

        DesktopPlatform->OpenDirectoryDialog(
            ParentWindowHandle,
            Title,
            DefaultPath,
            FolderPath
        );

        return FolderPath;
    }

    return FString();
}