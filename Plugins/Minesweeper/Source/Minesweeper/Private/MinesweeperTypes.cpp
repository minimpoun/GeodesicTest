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
			.Text_Lambda([this]
			{
				return bIsBomb ? LOCTEXT("Bomb", "B") : FText::GetEmpty();
				/*FFormatNamedArguments Args;
				Args.Add(TEXT("Flag"), FText::FromString(bIsFlagged ? TEXT("F") : TEXT("")));
				return FText::Format(LOCTEXT("FlagText", "{Flag}"), Args);*/
			})
		]
	];
}

auto SMinesweeperGridSlot::ExpandSlot() const -> void
{
	ButtonText->SetText(FText::FromString(FString::FromInt(GetNumAdjacentBombs())));
	
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

auto SMinesweeperGridSlot::GetNumAdjacentBombs() const -> uint8
{
	if (auto Slots = StaticCastSharedPtr<SUniformGridPanel>(GetParentWidget())->GetChildren()) [[likely]]
	{
		enum EPositionCheck : uint8
		{
			UP, DOWN, LEFT, RIGHT
		};
		
		[[nodiscard]] const auto CheckSlot = [this, &Slots](EPositionCheck InPosition)->uint8
		{
			// Used to reduce code coping
			struct FGridPosition_Internal
			{
				FGridPosition_Internal() = delete;
				FGridPosition_Internal(const uint8& InOffsetLeft, const uint8& InOffsetRight, const uint8& InOffset)
					: OffsetLeft(InOffsetLeft)
					, OffsetRight(InOffsetRight)
					, Offset(InOffset)
				{}
				
				uint8 OffsetLeft, OffsetRight, Offset;
			};
			
			[[nodiscard]] const auto GetSlotIndex = [this, &InPosition]()->FGridPosition_Internal
			{
				switch (InPosition)
				{
					case UP: return FGridPosition_Internal(
							GridPosition.GridSize * GridPosition.Column - 1 + GridPosition.Row + 1,
							GridPosition.GridSize * GridPosition.Column + 1 + GridPosition.Row + 1,
							GridPosition.GridSize * GridPosition.Column + GridPosition.Row + 1);
					case DOWN: return FGridPosition_Internal(
							GridPosition.GridSize * GridPosition.Column - 1 + GridPosition.Row - 1,
							GridPosition.GridSize * GridPosition.Column + 1 + GridPosition.Row - 1,
							GridPosition.GridSize * GridPosition.Column + GridPosition.Row - 1);
					case LEFT: return FGridPosition_Internal(
							GridPosition.GridSize * GridPosition.Column - 1 + GridPosition.Row - 1,
							GridPosition.GridSize * GridPosition.Column - 1 + GridPosition.Row + 1,
							GridPosition.GridSize * GridPosition.Column - 1 + GridPosition.Row);
					case RIGHT: return FGridPosition_Internal(
							GridPosition.GridSize * GridPosition.Column + 1 + GridPosition.Row - 1,
							GridPosition.GridSize * GridPosition.Column + 1 + GridPosition.Row + 1,
							GridPosition.GridSize * GridPosition.Column + 1 + GridPosition.Row);
					default: return {0, 0, 0};
				}
			};

			[[nodiscard]] const auto IsValidIndex = [this, &Slots](const uint8& Index)->bool
			{
				return (GridPosition.GridSize*GridPosition.GridSize) > Index && Index > 0;
			};

			[[nodiscard]] const auto GetSlotRef = [&Slots](const uint8& Index)->TSharedRef<SMinesweeperGridSlot>
			{
				return StaticCastSharedRef<SMinesweeperGridSlot>(Slots->GetChildAt(Index));
			};

			uint8 Count = 0;
			if (IsValidIndex(GetSlotIndex().Offset) && GetSlotRef(GetSlotIndex().Offset)->IsBombSlot())
			{
				Count++;
			}

			if (IsValidIndex(GetSlotIndex().OffsetLeft) && GetSlotRef(GetSlotIndex().OffsetLeft)->IsBombSlot())
			{
				Count++;
			}

			if (IsValidIndex(GetSlotIndex().OffsetRight) && GetSlotRef(GetSlotIndex().OffsetRight)->IsBombSlot())
			{
				Count++;
			}
			
			return Count;
		};

		return CheckSlot(UP) + CheckSlot(DOWN) + CheckSlot(LEFT) + CheckSlot(RIGHT);
	}

	return 0;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
