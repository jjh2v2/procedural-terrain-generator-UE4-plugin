// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ProceduralTerrainGeneratorEdModeToolkit.h"
#include "LandscapeFilter.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"
#include "UserWidget.h"

#define LOCTEXT_NAMESPACE "FProceduralTerrainGeneratorEdModeToolkit"

FProceduralTerrainGeneratorEdModeToolkit::FProceduralTerrainGeneratorEdModeToolkit()
{
}

void FProceduralTerrainGeneratorEdModeToolkit::OnFilterClassChanged(const UClass * NewClass)
{
	FilterClass = NewClass;

}

TArray<ALandscape*> FProceduralTerrainGeneratorEdModeToolkit::GetSelectedLandscapeActors()
{
	TArray<ALandscape*> rval;
	GEditor->GetSelectedActors()->GetSelectedObjects<ALandscape>(rval);
	return rval;
}

void FProceduralTerrainGeneratorEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{	
	FModeToolkit::Init(InitToolkitHost);
}

TSharedPtr<class SWidget> FProceduralTerrainGeneratorEdModeToolkit::GetInlineContent() const
{
	TSharedPtr<SWidget> Content;
	SAssignNew(Content, SBorder)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Fill)
	.Padding(FMargin(10.0f, 12.0f))
	[
		SNew(SScaleBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Stretch(EStretch::ScaleToFit)
		.StretchDirection(EStretchDirection::DownOnly)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			.Padding(0)
			[
				SNew(STextBlock)
				.AutoWrapText(false)
				.Justification(ETextJustify::Center)
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 14, EFontHinting::AutoLight))
				.Text(LOCTEXT("FProceduralTerrainGeneratorEdModeToolkit.Title", "Landscape Procedural Terrain Generator"))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(10)
			[
				SNew(SClassPropertyEntryBox)
				.AllowAbstract(false)
				.AllowNone(false)
				.MetaClass(ULandscapeFilter::StaticClass())
				.SelectedClass_Lambda([this]
				{
					return FilterClass;
				})
				.OnSetClass(this, &FProceduralTerrainGeneratorEdModeToolkit::OnFilterClassChanged)
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(10)
			[
				SNew(SButton)
				.IsEnabled_Lambda([this] 
				{
					return GetSelectedLandscapeActors().Num() > 0 && FilterClass != nullptr;
				})
				.OnClicked_Lambda([this] () -> FReply
				{
					const FText TransactionTitle = LOCTEXT("FProceduralTerrainGeneratorEdModeToolkit.UndoRedoApplyFiltersName", "Apply Filter");
					const FString TransactionNamespace = "ProceduralTerrainTool";

					ULandscapeFilter* CurrentFilterInstance = NewObject<ULandscapeFilter>(GetTransientPackage(), FilterClass->ClassDefaultObject->GetClass());
					FString FilterName = CurrentFilterInstance->GetClass()->GetName();
					UE_LOG(ProceduralTerrainGenerator, Log, TEXT("Applying filter %s"), *FilterName);
					for (auto Landscape : GetSelectedLandscapeActors())
					{
						GEditor->BeginTransaction(*TransactionNamespace, TransactionTitle, Landscape);
						UE_LOG(ProceduralTerrainGenerator, Log, TEXT("Applying filter %s to Landscape %s"), *FilterName, *(Landscape->GetName()));
						CurrentFilterInstance->ApplyFilter(Landscape, new FRandomStream(0xCAFE));
						GEditor->EndTransaction();
					}

					return FReply::Handled();
				})
				.Text(LOCTEXT("FProceduralTerrainGeneratorEdModeToolkit.ActivateFilter", "Activate Filter"))
			]
		]
	];

	return Content;
}

#undef LOCTEXT_NAMESPACE
