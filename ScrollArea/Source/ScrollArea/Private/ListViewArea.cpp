#include "ListViewArea.h"

TSharedRef<STableViewBase> UListViewArea::RebuildListWidget()
{
	TSharedRef<SListViewArea<UObject*>> Ref = ConstructListView<SListViewArea>();
	Ref->SetAllowMouseLeft(this->bEnableLeftClickScrolling);
	return Ref;
}

UUserWidget& UListViewArea::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (this->bEnableLeftClickScrolling)
	{
		return GenerateTypedEntry<UUserWidget, SListViewAreaObjectTableRow<UObject*>>(DesiredEntryClass, OwnerTable);
	}

	return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
}
