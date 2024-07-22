// Copyright (c) 2024, zabicode. All rights reserved.


#include "SlateWidgets/ExportThumbnailWidget.h"
#include "SlateBasics.h"

#include "DebugHeader.h"
#include "ThumbnailExportManager.h"
#include "DesktopPlatformModule.h" // Für FileOpener
#include "Misc/FileHelper.h"     // Für FFileHelper

void SExportThumbnailTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

    //DestinationFolderPathValue = InArgs._DestinationFolderPath;

	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
    TitleTextFont.Size = 20;

    DestinationFolderPathValueAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SExportThumbnailTab::GetDestinationFolderPathText));


    ComboBoxThumbnailFormatSourceItems.Add(MakeShared<FString>(".png"));
    ComboBoxThumbnailFormatSourceItems.Add(MakeShared<FString>(".jpeg"));
    ComboBoxThumbnailFormatSourceItems.Add(MakeShared<FString>(".bmp"));
    ComboBoxThumbnailResolutionSourceItems.Add(MakeShared<FString>("128x128"));
    ComboBoxThumbnailResolutionSourceItems.Add(MakeShared<FString>("256x256"));
    ComboBoxThumbnailResolutionSourceItems.Add(MakeShared<FString>("512x512"));
    ComboBoxThumbnailResolutionSourceItems.Add(MakeShared<FString>("1024x1024"));


	ChildSlot
		[

            // Main Vertical box
            SNew(SVerticalBox)

                // First vertical slot for title text
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(FMargin(0, 0, 0, 20)) // Bottom padding for spacing

                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Export Thumbnails")))
                        .Font(TitleTextFont)
                        .Justification(ETextJustify::Center)
                        .ColorAndOpacity(FColor::White)
                ]

                

                // Second vertical slot for first dropdown
                +SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center) // Align the content horizontally center
                .Padding(FMargin(0, 0, 0, 10)) // Bottom padding for spacing

                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("Select Format Option")))

                        ]

                        //Combo Box slot
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(10,0,0, 0)) // Padding on the left

                        [
                           
                            SNew(SComboBox < TSharedPtr <FString > >)
                            .OptionsSource(&ComboBoxThumbnailFormatSourceItems)
                            .OnGenerateWidget(this, &SExportThumbnailTab::OnGenerateComboContent)
                            .OnSelectionChanged(this, &SExportThumbnailTab::OnComboSelectionChanged)
                            [
                                SAssignNew(ComboDiplayTextBlockFormat, STextBlock)
                                    .Text(FText::FromString(TEXT("Export Format Options")))
                            ]

                        ]

                       
            
                ]

                

                // Third vertical slot for second dropdown
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center) // Align the content horizontally center
                .Padding(FMargin(0, 0, 0, 10)) // Bottom padding for spacing

                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString(TEXT("Select Resolution Option")))

                        ]


                        //Combo Box slot
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(10, 0, 0, 0)) // Padding on the left

                        [
                            SNew(SComboBox < TSharedPtr <FString > >)
                                .OptionsSource(&ComboBoxThumbnailResolutionSourceItems)
                                .OnGenerateWidget(this, &SExportThumbnailTab::OnGenerateComboContent)
                                .OnSelectionChanged(this, &SExportThumbnailTab::OnComboSelectionChanged2)
                                [
                                    SAssignNew(ComboDiplayTextBlockResolution, STextBlock)
                                        .Text(FText::FromString(TEXT("Export Resolution Options")))
                                ]
                        ]
                ]

                // Fourth vertical slot for file picker
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center) // Align the content horizontally center
                .Padding(FMargin(0, 0, 0, 20)) // Bottom padding for spacing

                [
                    SNew(SHorizontalBox)

                        //Combo Box slot
                        +SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                                .Text(FText::FromString(TEXT("Select Destination Folder")))
                                .OnClicked(this, &SExportThumbnailTab::OnSelectFolderButtonClicked)
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(FMargin(20, 0, 0, 0)) // Padding on the left

                        [
                            SNew(STextBlock)
                                .Text(DestinationFolderPathValueAttr)

                        ]

              
                ]

                // Fourth vertical slot for file picker
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Center) // Align the content horizontally center

                [
                    SNew(SButton)
                        .Text(FText::FromString(TEXT("Export Thumbnails")))
                        .OnClicked(this, &SExportThumbnailTab::OnExportThumbnailsClicked)
                        .ButtonColorAndOpacity(FColor::Green) // Orange Text

                        
                ]

                             
        ];
}

FText SExportThumbnailTab::GetDestinationFolderPathText() const
{
    return FText::FromString(DestinationFolderPathValue);
}

void SExportThumbnailTab::RefreshDestinationFolderPathDisplay(const FString& NewDestinationFolderPath)
{
    DestinationFolderPathValue = NewDestinationFolderPath; // Set new destination path

    // Trigger UI update
    if (DestinationFolderPathValueAttr.IsBound())
    {
        DestinationFolderPathValueAttr.Set(FText::FromString(DestinationFolderPathValue));
    }
    
    else
    {
        DestinationFolderPathValueAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SExportThumbnailTab::GetDestinationFolderPathText));
    }
    
}


/*
TSharedRef<SComboBox<TSharedPtr<FString>>> SExportThumbnailTab::ConstructComboBox()
{
    TSharedRef< SComboBox < TSharedPtr <FString > > > ConstructedComboBox =
        SNew(SComboBox < TSharedPtr <FString > >)
        .OptionsSource(&ComboBoxThumbnailFormatSourceItems)
        .OnGenerateWidget(this, &SExportThumbnailTab::OnGenerateComboContent)
        .OnSelectionChanged(this, &SExportThumbnailTab::OnComboSelectionChanged)
        [
            SAssignNew(ComboDiplayTextBlock, STextBlock)
                .Text(FText::FromString(TEXT("Export Resolution Options")))
        ];

    return ConstructedComboBox;
}

*/

/*
void SExportThumbnailTab::SetDestinationFolderPath(FString DestinationFolderPath)
{
    DestinationFolderPathValue = DestinationFolderPath;
}
*/

TSharedRef<SWidget> SExportThumbnailTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
    TSharedRef <STextBlock> ContructedComboText = SNew(STextBlock)
        .Text(FText::FromString(*SourceItem.Get()));

    return ContructedComboText;
}

void SExportThumbnailTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption,
    ESelectInfo::Type InSelectInfo)
{
    DebugHeader::Print(TEXT("AUSGEHEND: " + *SelectedOption.Get()), FColor::Red);


    ComboDiplayTextBlockFormat->SetText(FText::FromString(*SelectedOption.Get()));

    FThumbnailExportManagerModule& ThumbnailExportManagerModule =
        FModuleManager::LoadModuleChecked<FThumbnailExportManagerModule>(TEXT("ThumbnailExportManager"));;


    if (!SelectedOption.Get()->IsEmpty()) 
    {
        ThumbnailExportManagerModule.SetExportFormat(*SelectedOption.Get());
    }

}


void SExportThumbnailTab::OnComboSelectionChanged2(TSharedPtr<FString> SelectedOption,
    ESelectInfo::Type InSelectInfo)
{
    DebugHeader::Print(TEXT("AUSGEHEND: " + *SelectedOption.Get()), FColor::Red);


    ComboDiplayTextBlockResolution->SetText(FText::FromString(*SelectedOption.Get()));

    FThumbnailExportManagerModule& ThumbnailExportManagerModule =
        FModuleManager::LoadModuleChecked<FThumbnailExportManagerModule>(TEXT("ThumbnailExportManager"));;


    if (!SelectedOption.Get()->IsEmpty())
    {
        ThumbnailExportManagerModule.SetExportResolution(*SelectedOption.Get());
    }

}

FReply SExportThumbnailTab::OnSelectFolderButtonClicked()
{
    FString ExportPath = OpenFileDialog();

    if (ExportPath.IsEmpty())
    {
        DebugHeader::Print(TEXT("Please enter a valid destination folder"), FColor::Cyan);
    }

    DebugHeader::Print(ExportPath + TEXT(" is clicked"), FColor::Green);

    FThumbnailExportManagerModule& ThumbnailExportManagerModule =
        FModuleManager::LoadModuleChecked<FThumbnailExportManagerModule>(TEXT("ThumbnailExportManager"));;

    ThumbnailExportManagerModule.SetExportPath(ExportPath);

    RefreshDestinationFolderPathDisplay(ExportPath);
    
    return FReply::Handled();
}

FReply SExportThumbnailTab::OnExportThumbnailsClicked()
{
    DebugHeader::Print(TEXT("OnExportThumbnailsClicked is clicked"), FColor::Green);

    FThumbnailExportManagerModule& ThumbnailExportManagerModule =
        FModuleManager::LoadModuleChecked<FThumbnailExportManagerModule>(TEXT("ThumbnailExportManager"));;

    ThumbnailExportManagerModule.ExportAssetThumbnail();

    return FReply::Handled();
}


FString SExportThumbnailTab::OpenFileDialog()
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

#pragma endregion
