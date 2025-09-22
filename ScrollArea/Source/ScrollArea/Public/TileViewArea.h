#pragma once

#include "CoreMinimal.h"
#include "STileViewArea.h"
#include "Components/TileView.h"
#include "TileViewArea.generated.h"

/**
 * 瓦片视图滚动框区域
 * 1.允许使用鼠标左键进行滚动，需要自行设置
 * 2.支持虚拟化
 * 3.To make a widget usable as an entry in a TileView, it must inherit from the IUserObjectListEntry interface.
 */
UCLASS()
class SCROLLAREA_API UTileViewArea : public UTileView
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
