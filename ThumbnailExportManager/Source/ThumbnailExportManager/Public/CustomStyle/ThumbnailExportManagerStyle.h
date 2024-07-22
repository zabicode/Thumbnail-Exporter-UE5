// Copyright (c) 2024, zabicode. All rights reserved.

#pragma once

#include "Styling/SlateStyle.h"

class FThumbnailExportManagerStyle
{
public:
	static void InitializeIcons();

	static void ShutDown();

	static FName GetStyleSetName() { return StyleSetName; }

private:
	static FName StyleSetName;

	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();

	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;

};