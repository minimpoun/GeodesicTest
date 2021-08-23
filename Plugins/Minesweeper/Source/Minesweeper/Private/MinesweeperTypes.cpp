#include "MinesweeperTypes.h"

#include "SlateOptMacros.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FMinesweeperModule"

auto SMinesweeperGridButton::Construct(const FArguments& InArgs) -> void
{
	OnRightClicked = InArgs._OnRightClicked;

	Super::FArguments ButtonArgs;
	ButtonArgs._Content = InArgs._Content;
	ButtonArgs.OnClicked(InArgs._OnClicked);
	Super::Construct(ButtonArgs);
}

auto SMinesweeperGridButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) -> FReply
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

auto SMinesweeperGridSlot::Construct(const FArguments& InArgs) -> void
{
	GridPosition = GetMetaData<FMinesweeperMetaData>()->GridPosition;
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
			.Justification(ETextJustify::Center)
			.ShadowOffset(1.f)
			.HighlightColor(FColor::Red)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
			.Text_Lambda([this]
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("Flag"), FText::FromString(bIsFlagged ? TEXT("F") : TEXT("")));
				return FText::Format(LOCTEXT("FlagText", "{Flag}"), Args);
			})
		]
	];
}

auto SMinesweeperGridSlot::ExpandSlot() -> void
{
	const uint8 SlotValue = GetAdjBombCount();
	if (SlotValue == 0)
	{
		ExpandAdjSlots();
		return;
	}
	ButtonText->SetText(FText::FromString(FString::FromInt(SlotValue)));
	ButtonText->SetColorAndOpacity(GetNumberColor(SlotValue));
}

auto SMinesweeperGridSlot::SetBombSlot() -> void
{
	bIsBomb = true;
}

auto SMinesweeperGridSlot::FlagSlot() -> FReply
{
	bIsFlagged = !bIsFlagged;
	
	return FReply::Handled();
}

auto SMinesweeperGridSlot::OnSlotClicked() -> FReply
{
	if (bIsFlagged) return FReply::Handled();
	
	if (bIsBomb)
	{
		FReply Reply = FReply::Unhandled();
		ButtonText->SetText(LOCTEXT("BombText", "B"));
		
		FNotificationInfo NotificationInfo{LOCTEXT("GameOverNotification", "You hit a bomb! Game Over")};
		NotificationInfo.ExpireDuration = 2.5f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		
		if (OnGameOver.IsBound()) [[likely]]
		{
			Reply = OnGameOver.Execute();
			ensure(Reply.IsEventHandled() == true);
			return Reply;
		}

		return FReply::Unhandled();
	}

	ExpandSlot();
	return FReply::Handled();
}

auto SMinesweeperGridSlot::GetNumberColor(const int32& InNumber) -> FLinearColor
{
	switch (InNumber)
	{
		case 1: return FColor::Green;
		case 2: return FColor::Orange;
		default: return FColor::Red;
	}
}

auto SMinesweeperGridSlot::GetSlotIndex(const EPositionCheck& InPosition) const -> FGridPosition_Internal
{
	const int32 Index = GridPosition.GetGridHeight() * GridPosition.Column + GridPosition.Row;
	switch (InPosition)
	{
		case UP: return FGridPosition_Internal(
				Index - GridPosition.GetGridHeight() - 1,
				Index + GridPosition.GetGridHeight() - 1,
				Index - 1);
		case DOWN: return FGridPosition_Internal(
				Index - GridPosition.GetGridHeight() + 1,
				Index + GridPosition.GetGridHeight() + 1,
				Index + 1);
		case LEFT: return FGridPosition_Internal(
				0,
				0,
				Index - GridPosition.GetGridHeight());
		case RIGHT: return FGridPosition_Internal(
				0,
				0,
				Index + GridPosition.GetGridHeight());
		default: return {0, 0, 0};
	}
}

auto SMinesweeperGridSlot::GetAdjBombCount() const -> uint8
{
	if (auto Slots = StaticCastSharedPtr<SUniformGridPanel>(GetParentWidget())->GetChildren()) [[likely]]
	{
		[[nodiscard]] const auto CheckSlot = [this, &Slots](EPositionCheck InPosition)->uint8
		{
			if (InPosition == UP && GridPosition.Row == 0) return 0;
			if (InPosition == DOWN && GridPosition.Row == GridPosition.GridSize - 1) return 0;
			if (InPosition == LEFT && GridPosition.Column == 0) return 0;
			if (InPosition == RIGHT && GridPosition.Column == GridPosition.GridSize - 1) return 0;

			[[nodiscard]] const auto IsValidIndex = [this, &Slots](const int32& Index)->bool
			{
				return (GridPosition.GridSize*GridPosition.GetGridHeight()) > Index && Index > 0;
			};

			[[nodiscard]] const auto GetSlotRef = [&Slots](const int32& Index)->TSharedRef<SMinesweeperGridSlot>
			{
				return StaticCastSharedRef<SMinesweeperGridSlot>(Slots->GetChildAt(Index));
			};
			
			uint8 Count = 0;
			if (IsValidIndex(GetSlotIndex(InPosition).Offset) && GetSlotRef(GetSlotIndex(InPosition).Offset)->IsBombSlot())
			{
				Count++;
			}

			if (IsValidIndex(GetSlotIndex(InPosition).OffsetLeft) && GetSlotRef(GetSlotIndex(InPosition).OffsetLeft)->IsBombSlot() && InPosition != LEFT && InPosition != RIGHT)
			{
				Count++;
			}

			if (IsValidIndex(GetSlotIndex(InPosition).OffsetRight) && GetSlotRef(GetSlotIndex(InPosition).OffsetRight)->IsBombSlot() && InPosition != LEFT && InPosition != RIGHT)
			{
				Count++;
			}
			
			return Count;
		};

		return CheckSlot(UP) + CheckSlot(DOWN) + CheckSlot(LEFT) + CheckSlot(RIGHT);
	}

	return 0;
}

auto SMinesweeperGridSlot::ExpandAdjSlots() -> void
{
	if (bIsBomb || bIsExpanded) return;
	bIsExpanded = true;
	
	if (auto Slots = StaticCastSharedPtr<SUniformGridPanel>(GetParentWidget())->GetChildren())	[[likely]]
	{
		[[nodiscard]] const auto IsValidIndex = [this](const int32& Index)-> bool
		{
			return (GridPosition.GridSize * GridPosition.GetGridHeight()) > Index && Index > 0;
		};

		[[nodiscard]] const auto GetSlotRef = [&Slots](const int32& Index)-> TSharedRef<SMinesweeperGridSlot>
		{
			return StaticCastSharedRef<SMinesweeperGridSlot>(Slots->GetChildAt(Index));
		};
		
		if (GridPosition.Row != 0)
		{
			if (IsValidIndex(GetSlotIndex(UP).Offset) && !GetSlotRef(GetSlotIndex(UP).Offset)->IsBombSlot())
			{
				GetSlotRef(GetSlotIndex(UP).Offset)->ExpandSlot();
			}

			if (IsValidIndex(GetSlotIndex(UP).OffsetLeft) && !GetSlotRef(GetSlotIndex(UP).OffsetLeft)->IsBombSlot())
			{
				GetSlotRef(GetSlotIndex(UP).OffsetLeft)->ExpandSlot();
			}

			if (IsValidIndex(GetSlotIndex(UP).OffsetRight) && !GetSlotRef(GetSlotIndex(UP).OffsetRight)->IsBombSlot())
			{
				GetSlotRef(GetSlotIndex(UP).OffsetRight)->ExpandSlot();
			}
		}

		if (GridPosition.Row != GridPosition.GridSize - 1)
		{
			if (IsValidIndex(GetSlotIndex(DOWN).Offset) && !GetSlotRef(GetSlotIndex(DOWN).Offset)->IsBombSlot())
			{
				GetSlotRef(GetSlotIndex(DOWN).Offset)->ExpandSlot();
			}

			if (IsValidIndex(GetSlotIndex(DOWN).OffsetLeft) && !GetSlotRef(GetSlotIndex(DOWN).OffsetLeft)->IsBombSlot())
			{
				GetSlotRef(GetSlotIndex(DOWN).OffsetLeft)->ExpandSlot();
			}

			if (IsValidIndex(GetSlotIndex(DOWN).OffsetRight) && !GetSlotRef(GetSlotIndex(DOWN).OffsetRight)->IsBombSlot())
			{
				GetSlotRef(GetSlotIndex(DOWN).OffsetRight)->ExpandSlot();
			}
		}	
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
