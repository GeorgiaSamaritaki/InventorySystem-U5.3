// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInventory.h"

// Sets default values for this component's properties
UMyInventory::UMyInventory() {
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMyInventory::BeginPlay() {
	Super::BeginPlay();


}

//Adds to the appropriate inventory, if index is specified use that one, otherwise index is 0 
bool UMyInventory::AddToInventory(FSlot Item, int32 index) {
	TArray<FSlot>* Inventory = GetInventory(Item.ItemType);

	if (!Inventory) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid Inventory or Index out of bounds"));
		return false;
	}

	for (int i = index; i < GetInventorySize(Item.ItemType); i++) {
		if (AddItemToIndex(Inventory, Item, i)) return true;
	}

	return false;
}

FSlot UMyInventory::RemoveFromInventory(FSlot Item, int32 Index, int32 Quantity) {
	TArray<FSlot>* Inventory = GetInventory(Item.ItemType);

	if (!Inventory) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid Inventory or Index out of bounds"));
		return Item;
	}

	FSlot& Slot = (*Inventory)[Index]; // Reference to the slot at the specified index

	// Check if the slot contains the same item
	if (!Slot.SameItem(Item)) {
		UE_LOG(LogTemp, Warning, TEXT("Item mismatch: Cannot remove item that is not present in the slot"));
		return FSlot(); // Return an empty slot if the items don't match
	}

	// Create the removed item to return
	FSlot RemovedItem = Slot;
	RemovedItem.Quantity = Quantity;

	// Update the slot's quantity
	Slot.Quantity -= Quantity;

	// If the slot is now empty, reset it
	if (Slot.Quantity <= 0) Slot = FSlot();

	// Log the removal
	UE_LOG(LogTemp, Log, TEXT("Removed %d from index %d"), Quantity, Index);

	return RemovedItem; // Return the removed item
}

bool UMyInventory::AddItemToIndex(TArray<FSlot>* Inventory, FSlot Item, int32 Index) {
	FSlot& ExistingItem = (*Inventory)[Index];
	int32 NewQuantity = ExistingItem.Quantity + Item.Quantity;

	//We can add the item if the slot is empty or we can increase the stack 
	if (ExistingItem.IsEmpty() || (ExistingItem.SameItem(Item) && NewQuantity <= getStackSize(&ExistingItem))) {
		Item.Quantity = NewQuantity;
		ExistingItem = Item;

		UE_LOG(LogTemp, Warning, TEXT("Can add"));
		return true;
	}

	return false;
}

int32 UMyInventory::getStackSize(FSlot* ExistingItem) const {
	static const FString ContextString(TEXT("RowHandle Context"));

	if (ItemsDataTable) {
		FItemRow* Row = ItemsDataTable->FindRow<FItemRow>(ExistingItem->ItemID.RowName, ContextString);
		if (Row)
			return Row->StackSize;
	}
	return -1;
}


void UMyInventory::ResizeInventory(EItemTypes InventoryType) {
	TArray<FSlot>* inventory = GetInventory(InventoryType);
	int32 inventorySize = GetInventorySize(InventoryType);

	UE_LOG(LogTemp, Warning, TEXT("Resizing to %d"), inventorySize);
	if (inventory) inventory->SetNum(inventorySize);

}

TArray<FSlot>* UMyInventory::GetInventory(EItemTypes InventoryType) {
	switch (InventoryType) {
	case EItemTypes::Food:
		return &AllItems.Food;
	}

	return NULL;
}

int32 UMyInventory::GetInventorySize(EItemTypes InventoryType) const {
	switch (InventoryType) {
	case EItemTypes::Food:
		return AllItems.FoodInventorySize;
	}
	return 0;
}

int32 UMyInventory::SetInventorySize(EItemTypes InventoryType, int32 NewSize) {
	switch (InventoryType) {
	case EItemTypes::Food:
		return AllItems.FoodInventorySize = NewSize;
	}
	return NewSize;
}

// Called every frame
void UMyInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

