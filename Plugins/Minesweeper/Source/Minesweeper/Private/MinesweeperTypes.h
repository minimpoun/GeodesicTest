#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal(FReply, FOnRightClicked);
DECLARE_DELEGATE_RetVal(FReply, FOnGameOver);

struct FGridPositionData
{
	FGridPositionData() = default;
	FGridPositionData(const uint8& InColumn, const uint8& InRow, const uint8& InGridSize)
		: Column(InColumn)
		, Row(InRow)
		, GridSize(InGridSize)
	{}
	
	uint8 Column;
	uint8 Row;
	uint8 GridSize;
};

class FMinesweeperMetaData : public ISlateMetaData
{
public:

	SLATE_METADATA_TYPE(FMinesweeperMetaData, ISlateMetaData);
	
	FMinesweeperMetaData(const FGridPositionData& GridPosition)
	{
		this->GridPosition = GridPosition;
	}

	FGridPositionData GridPosition;
};

class SMinesweeperGridButton final : public SButton
{
	using Super = SButton;
	
	SLATE_BEGIN_ARGS(SMinesweeperGridButton) {}
		SLATE_EVENT(FOnClicked, OnClicked)
		SLATE_EVENT(FOnRightClicked, OnRightClicked)
		SLATE_DEFAULT_SLOT(FArguments, Content)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);

	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

protected:

	FOnRightClicked OnRightClicked;
};

class SMinesweeperGridSlot final : public SCompoundWidget
{
	using ThisClass = SMinesweeperGridSlot;
	
	SLATE_BEGIN_ARGS(SMinesweeperGridSlot){}
		SLATE_EVENT(FOnGameOver, OnGameOver)
	SLATE_END_ARGS()

	SMinesweeperGridSlot()
		: GridPosition()
		, bIsBomb(false)
		, bIsFlagged(false)
		, bIsExpanded(false)
	{}

	void Construct(const FArguments& InArgs);
	
	void ExpandSlot() const;
	void SetBombSlot();
	FORCEINLINE [[nodiscard]] auto IsBombSlot() const { return bIsBomb; }
	[[nodiscard]] auto FlagSlot() -> FReply;
	[[nodiscard]] auto OnSlotClicked() -> FReply;

protected:

	FOnGameOver OnGameOver;

private:
	
	[[nodiscard]] auto GetNumAdjacentBombs() const -> uint8;

	TSharedPtr<STextBlock> ButtonText;
	FGridPositionData GridPosition;
	uint8 bIsBomb:1;
	uint8 bIsFlagged:1;
	uint8 bIsExpanded:1;
};
