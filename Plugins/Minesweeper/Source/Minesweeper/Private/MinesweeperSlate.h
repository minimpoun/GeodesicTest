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

	[[nodiscard]] auto GetGridHeight() const -> int32;

	[[nodiscard]] auto ConstructSingleGridSlot(const int32& Column,
	                                           const int32& Row) -> TSharedRef<class SMinesweeperGridSlot>;
	[[nodiscard]] auto GenerateNewGrid() -> FReply;

	uint8 bDisableGridRatio:1;
	TSharedPtr<class SUniformGridPanel> MinesweeperGrid;
	int32 GridSize = 15, NumBombs = 30;
	int32 GridRows = GridSize;
	int32 BombsRemaining = NumBombs;
};
