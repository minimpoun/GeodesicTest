#include "MinesweeperSlate.h"

#include "Internationalization/BreakIterator.h"
#include "Widgets/Layout/SUniformGridPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FMinesweeperModule"

void SMinesweeperTabContent::Construct(const FArguments& InArgs)
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
				.SlotPadding(FMargin(1.f))
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

FReply SMinesweeperTabContent::GenerateNewGrid()
{
	MinesweeperGrid->ClearChildren();
	
	for (uint8 i{0}; i < GridSize; i++)
	{
		for (uint8 j{0}; j < GridSize; j++)
		{
			MinesweeperGrid->AddSlot(i, j)
			[
				SNew(SButton)
			];
		}
	}

	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

