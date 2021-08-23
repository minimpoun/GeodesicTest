#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal(FReply, FOnRightClicked);
DECLARE_DELEGATE_RetVal(FReply, FOnGameOver);

struct FGridPositionData
{
	FGridPositionData() = default;
	FGridPositionData(const int32& InColumn, const int32& InRow, const int32& InGridSize, const int32& InGridHeight, const uint8& InUseGridHeight)
		: Column(InColumn)
		, Row(InRow)
		, GridSize(InGridSize)
		, GridHeight(InGridHeight)
		, bUseGridHeight(InUseGridHeight)
	{}
	
	int32 Column;
	int32 Row;
	int32 GridSize;
	int32 GridHeight;
	uint8 bUseGridHeight:1;

	auto GetGridHeight() const -> int32
	{
		return bUseGridHeight ? GridHeight : GridSize;
	}

	FString ToString() const
	{
		return FString::Format(TEXT("Column: {0} -- Row: {1} -- GridSize: {2}"), {Column, Row, GridSize});
	}
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
	
	void ExpandSlot();
	void SetBombSlot();
	FORCEINLINE [[nodiscard]] auto IsBombSlot() const { return bIsBomb; }
	[[nodiscard]] auto FlagSlot() -> FReply;
	[[nodiscard]] auto OnSlotClicked() -> FReply;

protected:

	FOnGameOver OnGameOver;

private:

	enum EPositionCheck : uint8
	{
		UP, DOWN, LEFT, RIGHT
	};

	struct FGridPosition_Internal
	{
		FGridPosition_Internal() = delete;
		FGridPosition_Internal(const int32& InOffsetLeft, const int32& InOffsetRight, const int32& InOffset)
			: OffsetLeft(InOffsetLeft)
			, OffsetRight(InOffsetRight)
			, Offset(InOffset)
		{}

		int32 OffsetLeft, OffsetRight, Offset;
	};

	[[nodiscard]] auto GetSlotIndex(const EPositionCheck& InPosition) const -> FGridPosition_Internal;
	
	auto GetAdjBombCount() const -> uint8;
	auto ExpandAdjSlots() -> void;

	TSharedPtr<STextBlock> ButtonText;
	FGridPositionData GridPosition;
	uint8 bIsBomb:1;
	uint8 bIsFlagged:1;
	uint8 bIsExpanded:1;
};
