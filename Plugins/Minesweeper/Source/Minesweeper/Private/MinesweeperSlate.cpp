#include "MinesweeperSlate.h"

#include "MinesweeperTypes.h"
#include "SlateOptMacros.h"

#include "Framework/Notifications/NotificationManager.h"

#include "Internationalization/BreakIterator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FMinesweeperModule"

auto SMinesweeperTabContent::Construct(const FArguments& InArgs) -> void
{
	bDisableGridRatio = false;
#define NEW_HSLOT\
 SHorizontalBox::Slot()\
.Padding(1.f)\
.AutoWidth()\
.VAlign(VAlign_Top)
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ NEW_HSLOT
			[
				SNew(STextBlock)
				.Text(LOCTEXT("GridText", "Grid Size"))
				.Margin(FMargin(0.f, 0.f, 1.f, 0.))
			]
			+ NEW_HSLOT
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(100)
				.HintText(LOCTEXT("GridHintEditableText", "Enter Grid Size"))
				.Text(LOCTEXT("GridSizeText", "15"))
				.OnTextChanged_Lambda([this](const FText& InText)
				 {
					GridSize = FCString::Atoi(*InText.ToString());
				 })
				 .Padding(FMargin(0.f, 0.f, 1.f, 0.f))
			]
			+ NEW_HSLOT
			[
				SNew(STextBlock)
				.Text(LOCTEXT("RowText", "Grid Height"))
				.Visibility_Lambda([this]()
                {
					return bDisableGridRatio ? EVisibility::Visible : EVisibility::Collapsed;
                })
                .Margin(FMargin(0.f, 0.f, 1.f, 0.))
			]
			+ NEW_HSLOT
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(100)
				.HintText(LOCTEXT("RowHintText", "Enter Grid Height"))
				.Text(LOCTEXT("RowEditableText", "15"))
				.Visibility_Lambda([this]()
                 {
					return bDisableGridRatio ? EVisibility::Visible : EVisibility::Collapsed;
                 })
				.OnTextChanged_Lambda([this](const FText& InText)
                 {
					GridRows = FCString::Atoi(*InText.ToString());
                 })
                 .Padding(FMargin(0.f, 0.f, 1.f, 0.f))
			]
			+ NEW_HSLOT
			[
				SNew(STextBlock)
				.Text(LOCTEXT("GridRatioButton", "Maintain Grid Ratio"))
				.Margin(FMargin(0.f, 0.f, 1.f, 0.))
			]
			+ NEW_HSLOT
			[
				SNew(SCheckBox)
				.IsChecked(true)
				.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState)
                {
 	               bDisableGridRatio = NewState != ECheckBoxState::Checked;
                })
			]
			+ NEW_HSLOT
			[
				SNew(SSpacer)
				.Size(FVector2D(2.f, 0.f))
			]
			+ NEW_HSLOT
			[
				SNew(STextBlock)
				.Text(LOCTEXT("BombText", "Number of Bombs"))
				.Margin(FMargin(0.f, 0.f, 1.f, 0.))
			]
			+ NEW_HSLOT
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(100)
				.Text(LOCTEXT("NumBombText", "30"))
                .HintText(LOCTEXT("NumBombHintEditableText", "Enter Number of Bombs"))
                .OnTextChanged_Lambda([this](const FText& InText)
                {
					NumBombs = FCString::Atoi(*InText.ToString());
                })
			]
			+ NEW_HSLOT
			[
				SNew(SButton)
				.Text(LOCTEXT("GenButtonText", "Generate New Grid"))
				.OnClicked(this, &ThisClass::GenerateNewGrid)
			]
		]
		+ SVerticalBox::Slot()
		  .VAlign(VAlign_Fill)
		  .Padding(FMargin(1.f))
		  .FillHeight(1.f)
		[
			SAssignNew(MinesweeperGrid, SUniformGridPanel)
			.SlotPadding(1.5f)
		]
	];

#undef NEW_HSLOT
}

auto SMinesweeperTabContent::GetGridHeight() const -> int32
{
	return bDisableGridRatio ? GridRows : GridSize;
}

auto SMinesweeperTabContent::ConstructSingleGridSlot(const int32& Column,
                                                     const int32& Row) -> TSharedRef<SMinesweeperGridSlot>
{
	static int32 GUID = 0;
	GUID++;
	return SNew(SMinesweeperGridSlot)
		.Tag(*FString::FromInt(GUID))
		.AddMetaData<FMinesweeperMetaData>(
			FMinesweeperMetaData(FGridPositionData(Column, Row, GridSize, GetGridHeight(), bDisableGridRatio)))
		.OnGameOver_Raw(this, &ThisClass::GenerateNewGrid);
}

auto SMinesweeperTabContent::GenerateNewGrid() -> FReply
{
	MinesweeperGrid->ClearChildren();
	BombsRemaining = NumBombs;
	
	for (auto Column{0}; Column < GridSize; Column++)
	{
		for (auto Row{0}; Row < GetGridHeight(); Row++)
		{
			MinesweeperGrid->AddSlot(Column, Row)
			[
				ConstructSingleGridSlot(Column, Row)
			];
		}
	}
	
	volatile int32 GridIndex = 0;
	while (GridIndex < GridSize*GetGridHeight() && BombsRemaining > 0)
	{
		const int32 Random = FMath::Rand() % (GridSize*GetGridHeight());
		const int32 RandCol = Random / GridSize;
		const int32 RandRow = Random % GetGridHeight();
		const int32 FoundIndex = GridSize * RandCol + RandRow;
	
		StaticCastSharedRef<SMinesweeperGridSlot>(MinesweeperGrid->GetChildren()->GetChildAt(FoundIndex))->SetBombSlot();

		BombsRemaining--;
		GridIndex++;
	}

	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(LOCTEXT("GameStartNotification", "New game started, good luck!")));
	
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

