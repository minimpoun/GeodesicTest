#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SUniformGridPanel.h"

class SMinesweeperTabContent final : public SCompoundWidget
{
	using ThisClass = SMinesweeperTabContent;
	
	SLATE_BEGIN_ARGS(SMinesweeperTabContent) {}
	SLATE_END_ARGS()

	auto Construct(const FArguments& InArgs) -> void;

private:

	[[nodiscard]] auto ConstructSingleGridSlot(const int32& Column,
	                                           const int32& Row) -> TSharedRef<class SMinesweeperGridSlot>;
	[[nodiscard]] auto GenerateNewGrid() -> FReply;

	TSharedPtr<class SUniformGridPanel> MinesweeperGrid;
	uint8 GridSize = 5, NumBombs = 5;
	uint8 BombsRemaining = NumBombs;
};
