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
	const FText InstructionText = LOCTEXT("InstructText", "The purpose of the game is to open all the cells of the board which do not contain a bomb. You lose if you set off a bomb cell. Every non-bomb cell you open will tell you the total number of bombs in the eight neighboring cells. Once you are sure that a cell contains a bomb, you can right-click to put a flag on it as a reminder.");
	
#define NEW_HSLOT\
 SHorizontalBox::Slot()\
.Padding(1.f)\
.AutoWidth()\
.VAlign(VAlign_Top)
	
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ NEW_HSLOT
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
				]
				+ NEW_HSLOT
				[
					SNew(SEditableTextBox)
					.MinDesiredWidth(100)
					.HintText(LOCTEXT("GridHintEditableText", "Enter Grid Size"))
					.Text(LOCTEXT("GridSizeText", "5"))
					.ToolTipText(LOCTEXT("GridSizeTooltip", "Min Size: 2 -- Max Size: 20"))
					.OnTextChanged_Lambda([this](const FText& InText)
					{
						GridSize = FCString::Atoi(*InText.ToString());
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
				]
				+ NEW_HSLOT
				[
					SNew(SEditableTextBox)
					.MinDesiredWidth(100)
					.Text(LOCTEXT("NumBombText", "5"))
                    .HintText(LOCTEXT("NumBombHintEditableText", "Enter Number of Bombs"))
                    .ToolTipText(LOCTEXT("NumBombTooltip", "Min: 1 -- Max: (GridSize^2) - 2"))
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
		]
		+ SHorizontalBox::Slot()
		  .Padding(1.f)
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		  .HAlign(HAlign_Right)
		[
			SNew(STextBlock)
			.Text(InstructionText)
			.WrappingPolicy(ETextWrappingPolicy::DefaultWrapping)
			.WrapTextAt(512.0f)
			.Justification(ETextJustify::Left)
			.AutoWrapText(true)
			.LineBreakPolicy(FBreakIterator::CreateWordBreakIterator())
		]
	];

#undef NEW_HSLOT
}

auto SMinesweeperTabContent::ConstructSingleGridSlot(const int32& Column,
                                                     const int32& Row) -> TSharedRef<SMinesweeperGridSlot>
{
	static int32 GUID = 0;
	GUID++;
	return SNew(SMinesweeperGridSlot)
		.Tag(*FString::FromInt(GUID))
		.AddMetaData<FMinesweeperMetaData>(FMinesweeperMetaData(FGridPositionData(Column, Row, GridSize)))
		.OnGameOver_Raw(this, &ThisClass::GenerateNewGrid);
}

auto SMinesweeperTabContent::GenerateNewGrid() -> FReply
{
	MinesweeperGrid->ClearChildren();
	BombsRemaining = NumBombs;
	
	for (auto i{0}; i < GridSize; i++)
	{
		for (auto j{0}; j < GridSize; j++)
		{
			MinesweeperGrid->AddSlot(i, j)
			[
				ConstructSingleGridSlot(i, j)
			];
		}
	}
	
	volatile int32 GridIndex = 0;
	while (GridIndex < GridSize*GridSize && BombsRemaining > 0)
	{
		const int32 Random = FMath::Rand() % (GridSize*GridSize);
		const int32 RandCol = Random / GridSize;
		const int32 RandRow = Random % GridSize;
		const int32 FoundIndex = GridSize * RandCol + RandRow;
	
		StaticCastSharedRef<SMinesweeperGridSlot>(MinesweeperGrid->GetChildren()->GetChildAt(FoundIndex))->SetBombSlot();

		BombsRemaining--;
		GridIndex++;
	}

	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(LOCTEXT("GameStartNotification", "New game started, good luck!")));
	
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

