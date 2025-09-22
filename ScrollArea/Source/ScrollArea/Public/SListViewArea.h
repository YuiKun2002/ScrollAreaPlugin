// Copy Right ZeroSoul

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "Components/ListView.h"
#include "Slate/SObjectWidget.h"
#include "Slate/SObjectTableRow.h"

/*
	Engine->input
	No Capture
	Do Not Lock
*/


template <typename ItemType>
class SListViewArea : public SListView<ItemType> {

public:

	//允许左键交互
	FORCEINLINE	void SetAllowMouseLeft(bool Value) { this->bAllowMouseLeft = Value; }

protected:
	//是否允许左键交互
	bool bAllowMouseLeft = false;
	//左键开始交互
	bool bMouseLeftInteraction = false;
	//左键剩余量
	float AmountWhileMouseLeft = 0.f;

protected:

	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (!MouseEvent.IsTouchEvent() && this->bAllowMouseLeft)
		{
			if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			{
				// Clear any inertia
				this->InertialScrollManager.ClearScrollVelocity();
				AmountWhileMouseLeft = 0;
				bMouseLeftInteraction = true;

				return FReply::Unhandled();
			}
		}
		return SListView<ItemType>::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
	}

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (!MouseEvent.IsTouchEvent() && this->bAllowMouseLeft)
		{
			if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			{
				bMouseLeftInteraction = false;
				AmountWhileMouseLeft = 0.f;
				if (this->HasMouseCapture())
				{
					return FReply::Unhandled().ReleaseMouseCapture();
				}
			}
		}
		return SListView<ItemType>::OnMouseButtonUp(MyGeometry, MouseEvent);
	}

	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (!MouseEvent.IsTouchEvent() && this->bAllowMouseLeft)
		{
			if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && bMouseLeftInteraction)
			{
				FTableViewDimensions CursorDeltaDimensions(this->Orientation, MouseEvent.GetCursorDelta());
				CursorDeltaDimensions.LineAxis = 0.f;
				const float ScrollByAmount = CursorDeltaDimensions.ScrollAxis / MyGeometry.Scale;

				AmountWhileMouseLeft += FMath::Abs(ScrollByAmount);

				if (this->IsLeftScrolling())
				{
					// Make sure the active timer is registered to update the inertial scroll
					if (!this->bIsScrollingActiveTimerRegistered)
					{
						this->bIsScrollingActiveTimerRegistered = true;
						this->RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SListViewArea::UpdateMouseLeftInertialScroll));
					}


					this->TickScrollDelta -= ScrollByAmount;

					const float AmountScrolled = this->ScrollBy(MyGeometry, -ScrollByAmount, this->AllowOverscroll);

					FReply Reply = FReply::Unhandled();

					if (this->HasMouseCapture() == false)
					{
						Reply.UseHighPrecisionMouseMovement(this->AsShared());
					}

					return Reply;
				}
			}
		}

		return SListView<ItemType>::OnMouseMove(MyGeometry, MouseEvent);
	}

private:

	bool IsLeftScrolling() const
	{
		return AmountWhileMouseLeft >= FSlateApplication::Get().GetDragTriggerDistance() &&
			(this->ScrollBar->IsNeeded() || this->AllowOverscroll == EAllowOverscroll::Yes);
	}

	EActiveTimerReturnType UpdateMouseLeftInertialScroll(double InCurrentTime, float InDeltaTime)
	{
		bool bKeepTicking = false;
		if (this->ItemsPanel.IsValid())
		{
			if (bMouseLeftInteraction)
			{
				bKeepTicking = true;

				if (this->CanUseInertialScroll(this->TickScrollDelta))
				{
					this->InertialScrollManager.AddScrollSample(this->TickScrollDelta, InCurrentTime);
				}
			}
			else
			{
				this->InertialScrollManager.UpdateScrollVelocity(InDeltaTime);
				const float ScrollVelocity = this->InertialScrollManager.GetScrollVelocity();

				if (ScrollVelocity != 0.f)
				{
					if (this->CanUseInertialScroll(ScrollVelocity))
					{
						bKeepTicking = true;
						this->ScrollBy(this->GetTickSpaceGeometry(), ScrollVelocity * InDeltaTime, this->AllowOverscroll);
					}
					else
					{
						this->InertialScrollManager.ClearScrollVelocity();
					}
				}

				if (this->AllowOverscroll == EAllowOverscroll::Yes)
				{
					if (this->Overscroll.GetOverscroll(this->GetTickSpaceGeometry()) != 0.0f)
					{
						bKeepTicking = true;
						this->RequestLayoutRefresh();
					}

					this->Overscroll.UpdateOverscroll(InDeltaTime);
				}
			}

			this->TickScrollDelta = 0.f;
		}

		this->bIsScrollingActiveTimerRegistered = bKeepTicking;
		return bKeepTicking ? EActiveTimerReturnType::Continue : EActiveTimerReturnType::Stop;
	}
};

template <typename ItemType>
class SListViewAreaObjectTableRow : public SObjectTableRow<ItemType>
{
public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		return SObjectWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
	}

	// remove most steps in Parent Class, this only handle the SelectionMode::Single
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		FReply Reply = SObjectWidget::OnMouseButtonUp(MyGeometry, MouseEvent);

		if (!Reply.IsEventHandled())
		{
			TSharedPtr<ITypedTableView<ItemType>> OwnerTable = this->OwnerTablePtr.Pin();
			auto MyItemPtr = OwnerTable ? this->GetItemForThis(OwnerTable.ToSharedRef()) : nullptr;
			if (MyItemPtr)
			{
				const ESelectionMode::Type SelectionMode = this->GetSelectionMode();
				if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
				{
					bool bSignalSelectionChanged = false;
					const ItemType& MyItem = *MyItemPtr;
					if (this->IsItemSelectable() && MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
					{
						if (SelectionMode == ESelectionMode::Single)
						{
							bSignalSelectionChanged = true;
						}
					}

					if (bSignalSelectionChanged)
					{
						OwnerTable->Private_ClearSelection();
						OwnerTable->Private_SetItemSelection(MyItem, true, true);
						OwnerTable->Private_SignalSelectionChanged(ESelectInfo::OnMouseClick);
						Reply = FReply::Handled();
					}

					if (OwnerTable->Private_OnItemClicked(*MyItemPtr))
					{
						Reply = FReply::Handled();
					}

					Reply = Reply.ReleaseMouseCapture();
				}

			}
		}

		return Reply;
	}
};
