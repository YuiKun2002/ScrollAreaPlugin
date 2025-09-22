// Copy Right ZeroSoul

#pragma once

#include "CoreMinimal.h"
#include "SListViewArea.h"
#include "UObject/ObjectPtr.h"
#include "Slate/SObjectWidget.h"
#include "Slate/SObjectTableRow.h"
#include "Widgets/Views/STileView.h"

//瓦片视图列表
template <typename ItemType>
class STileViewArea : public STileView<ItemType> {

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
		return STileView<ItemType>::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
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
		return STileView<ItemType>::OnMouseButtonUp(MyGeometry, MouseEvent);
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
						this->RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &STileViewArea::UpdateMouseLeftInertialScroll));
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

		return STileView<ItemType>::OnMouseMove(MyGeometry, MouseEvent);
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
