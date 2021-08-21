#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SUniformGridPanel.h"

class SMinesweeperTabContent final : public SCompoundWidget
{
	using ThisClass = SMinesweeperTabContent;
	
	SLATE_BEGIN_ARGS(SMinesweeperTabContent) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	FReply GenerateNewGrid();

	TSharedPtr<class SUniformGridPanel> MinesweeperGrid;
	TSet<class SWidget*> GridButtons;
	uint8 GridSize = 5, NumBombs = 5;
};
