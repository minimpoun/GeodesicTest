#include "MinesweeperTypes.h"

#include "SlateOptMacros.h"

#include "Framework/Notifications/NotificationManager.h"

#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FMinesweeperModule"

void SMinesweeperGridButton::Construct(const FArguments& InArgs)
{
	OnRightClicked = InArgs._OnRightClicked;

	Super::FArguments ButtonArgs;
	ButtonArgs._Content = InArgs._Content;
	ButtonArgs.OnClicked(InArgs._OnClicked);
	Super::Construct(ButtonArgs);
}

FReply SMinesweeperGridButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled() && MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FReply Reply = FReply::Unhandled();
		if (OnRightClicked.IsBound())
		{
			Reply = OnRightClicked.Execute();
			ensure(Reply.IsEventHandled());
			return Reply;
		}
	}
	
	return Super::OnMouseButtonDown(MyGeometry, MouseEvent);
}

void SMinesweeperGridSlot::Construct(const FArguments& InArgs)
{
	GridPosition = GetMetaData<FMinesweeperMetaData>()->GridPosition;
	bIsBomb = GetMetaData<FMinesweeperMetaData>()->bIsBomb;
	OnGameOver = InArgs._OnGameOver;

	ChildSlot
	[
		SNew(SMinesweeperGridButton)
		.OnRightClicked_Raw(this, &ThisClass::FlagSlot)
		.OnClicked_Raw(this, &ThisClass::OnSlotClicked)
		.IsEnabled_Lambda([this]
		{
			return !bIsExpanded;
		})
		[
			SAssignNew(ButtonText, STextBlock)
		]
	];
}

void SMinesweeperGridSlot::ExpandSlot()
{
}

FReply SMinesweeperGridSlot::FlagSlot()
{
	bIsFlagged = !bIsFlagged;
	ButtonText->SetText(bIsFlagged ? LOCTEXT("FlagText", "") : LOCTEXT("FlagText", "F"));
	return FReply::Handled();
}

FReply SMinesweeperGridSlot::OnSlotClicked()
{
	if (bIsFlagged) return FReply::Handled();
	if (bIsBomb)
	{
		FReply Reply = FReply::Unhandled();
		ButtonText->SetText(LOCTEXT("BombText", "B"));
		FSlateNotificationManager::Get().AddNotification(FNotificationInfo(LOCTEXT("GameOverNotification", "You hit a bomb! Game Over")));
		if (OnGameOver.IsBound())
		{
			Reply = OnGameOver.Execute();
			ensure(Reply.IsEventHandled() == true);
			return Reply;
		}

		return FReply::Unhandled();
	}
	
	return FReply::Handled();
}

uint8 SMinesweeperGridSlot::FindNumAdjacentBombs() const
{
	return 0;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
