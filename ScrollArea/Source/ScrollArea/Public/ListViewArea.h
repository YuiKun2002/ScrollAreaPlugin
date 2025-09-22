#pragma once

#include "CoreMinimal.h"
#include "SListViewArea.h"
#include "ListViewArea.generated.h"

/**
 * 列表视图滚动框区域
 * 1.允许使用鼠标左键进行滚动，需要自行设置
 * 2.支持虚拟化
 * 3.To make a widget usable as an entry in a ListView, it must inherit from the IUserObjectListEntry interface.
 * 
 * An important distinction to keep in mind here is "Item" vs. "Entry"
 * The list itself is based on a list of n items, but only creates as many entry widgets as can fit on screen.
 * For example, a scrolling ListView of 200 items with 5 currently visible will only have created 5 entry widgets.
 * 
 */
UCLASS()
class SCROLLAREA_API UListViewArea : public UListView
{
	GENERATED_BODY()

public:

	//允许鼠标左键滚动列表
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Scrolling)
	bool bEnableLeftClickScrolling = false;

public:

	virtual TSharedRef<STableViewBase> RebuildListWidget() override;

	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
};
