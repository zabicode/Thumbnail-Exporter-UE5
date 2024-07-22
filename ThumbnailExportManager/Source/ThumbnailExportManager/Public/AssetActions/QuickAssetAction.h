// Copyright (c) 2024, zabicode. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "QuickAssetAction.generated.h"

UENUM(BlueprintType)
enum class EThumbnailFormat : uint8
{
	PNG UMETA(DisplayName = ".png"),
	JPEG UMETA(DisplayName = ".jpeg"),
	BMP UMETA(DisplayName = ".bmp")
};

UENUM(BlueprintType)
enum class EThumbnailResolution : uint8
{
	LOW UMETA(DisplayName = "128x128"),
	MEDIUM UMETA(DisplayName = "256x256"),
	HIGH UMETA(DisplayName = "512x512"),
	MEGA UMETA(DisplayName = "1024x1024")
};

struct FImageResolution
{
	int32 ImageHeight;
	int32 ImageWidth;

	FImageResolution()
		: ImageWidth(256), ImageHeight(256) {}

	FImageResolution(int32 InWidth, int32 InHeight)
		: ImageWidth(InWidth), ImageHeight(InHeight) {}
};

UCLASS()
class THUMBNAILEXPORTMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:

	UFUNCTION(CallInEditor)
	void ExportAssetThumbnail(EThumbnailFormat Format, EThumbnailResolution Resolution);

	FString OpenFileDialog();
	
};
