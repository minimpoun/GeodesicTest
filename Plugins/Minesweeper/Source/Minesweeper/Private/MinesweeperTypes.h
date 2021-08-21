#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal(FReply, FOnRightClicked);
DECLARE_DELEGATE_RetVal(FReply, FOnGameOver);

class FMinesweeperMetaData : public ISlateMetaData
{
public:

	SLATE_METADATA_TYPE(FMinesweeperMetaData, ISlateMetaData);
	
	FMinesweeperMetaData(const FVector2D& GridPosition, const bool& bIsBomb)
	{
		this->GridPosition = GridPosition;
		this->bIsBomb = bIsBomb;
	}

	FVector2D GridPosition;
	bool bIsBomb;
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

	void Construct(const FArguments& InArgs);
	
	void ExpandSlot();
	FReply FlagSlot();
	FReply OnSlotClicked();

protected:

	FOnGameOver OnGameOver;

private:

	uint8 FindNumAdjacentBombs() const;

	TSharedPtr<STextBlock> ButtonText;
	FVector2D GridPosition;
	uint8 bIsBomb:1;
	uint8 bIsFlagged:1;
	uint8 bIsExpanded:1;
	
};