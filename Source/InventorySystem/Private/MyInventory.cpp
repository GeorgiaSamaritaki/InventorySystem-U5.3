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

/**
 * Adds an item to the inventory.
 *
 * @param Item The item to add.
 * @return True if the item was successfully added, false otherwise.
 */
bool UMyInventory::AddToInventory(FSlot Item) {
	TArray<FSlot>* Inventory = GetInventory(Item.ItemType);

	if (!Inventory) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid Inventory or Index out of bounds"));
		return false;
	}

	for (int i = 0; i < GetInventorySize(Item.ItemType); i++) {
		if (AddItemToIndex(Inventory, Item, i)) return true;
	}

	return false;
}

/**
 * Adds an item to a specific index in the inventory.
 *
 * @param Item The item to add.
 * @param Index The index to add the item at.
 * @return True if the item was successfully added, false otherwise.
 */
bool UMyInventory::AddToInventoryOnIndex(FSlot Item, int32 Index) {
	TArray<FSlot>* Inventory = GetInventory(Item.ItemType);

	if (!Inventory) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid Inventory or Index out of bounds"));
		return false;
	}

	return AddItemToIndex(Inventory, Item, Index);
}

/**
 * Swaps two items in the inventory.
 *
 * @param InventoryType The type of inventory to perform the swap in.
 * @param IndexA The index of the first item to swap.
 * @param IndexB The index of the second item to swap.
 * @return True if the swap was successful, false otherwise.
 */
bool UMyInventory::SwapItems(EItemTypes InventoryType, int32 IndexA, int32 IndexB) {
	TArray<FSlot>* Inventory = GetInventory(InventoryType);

	// Validate inventory and indices
	if (!Inventory || !Inventory->IsValidIndex(IndexA) || !Inventory->IsValidIndex(IndexB)) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid Inventory or Index out of bounds"));
		return false;
	}

	// References to the slots being swapped
	FSlot& SlotA = (*Inventory)[IndexA];
	FSlot& SlotB = (*Inventory)[IndexB];

	// Log details before the swap
	//UE_LOG(LogTemp, Log, TEXT("Swapping Slot A (Index %d): %s (Quantity: %d) with Slot B (Index %d): %s (Quantity: %d)"),
	//    IndexA, *SlotA.ItemID.RowName, SlotA.Quantity, IndexB, *SlotB.ItemID.RowName, SlotB.Quantity);

	// Swap the slots
	FSlot Temp = SlotA;
	SlotA = SlotB;
	SlotB = Temp;

	return true;
}

/**
 * Adds an item to a specific index in the inventory.
 *
 * @param Inventory The inventory to add the item to.
 * @param Item The item to add.
 * @param Index The index to add the item at.
 * @return True if the item was successfully added, false otherwise.
 */
bool UMyInventory::AddItemToIndex(TArray<FSlot>* Inventory, FSlot Item, int32 Index) {
	FSlot& ExistingItem = (*Inventory)[Index];
	int32 NewQuantity = ExistingItem.Quantity + Item.Quantity;

	// We can add the item if the slot is empty or we can increase the stack 
	if (ExistingItem.IsEmpty() || (ExistingItem.SameItem(Item) && NewQuantity <= getStackSize(&ExistingItem))) {
		Item.Quantity = NewQuantity;
		ExistingItem = Item;

		UE_LOG(LogTemp, Warning, TEXT("Can add"));
		return true;
	}

	return false;
}

/**
 * Removes a specified quantity of an item from the inventory.
 * The item in the inventory is adjusted and a new item is returned.
 *
 * @param Item The item to remove.
 * @param Index The index to remove the item from.
 * @param Quantity The quantity to remove.
 * @return The removed item.
 */
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

	UE_LOG(LogTemp, Warning, TEXT("Quantity of slot before removal %d"), Slot.Quantity);

	// Update the slot's quantity
	Slot.Quantity -= Quantity;

	UE_LOG(LogTemp, Warning, TEXT("Quantity of slot after removal %d"), Slot.Quantity);

	// If the slot is now empty, reset it
	if (Slot.Quantity <= 0) Slot = FSlot();

	// Log the removal
	UE_LOG(LogTemp, Log, TEXT("Removed %d from index %d"), Quantity, Index);

	return RemovedItem; // Return the removed item
}

/**
 * Sorts the inventory based on the specified sorting mode.
 *
 * @param InventoryType The type of inventory to sort.
 * @param SortMode The sorting mode to apply.
 */
void UMyInventory::SortInventory(EItemTypes InventoryType, EInventorySortMode SortMode) {
	TArray<FSlot>* Inventory = GetInventory(InventoryType);

	if (!Inventory) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid Inventory"));
		return;
	}

	// Lambda to define sorting criteria
	Inventory->Sort([SortMode](const FSlot& A, const FSlot& B) {

		// Prioritize non-zero quantity slots over zero-quantity slots
		if (A.Quantity == 0 && B.Quantity != 0) {
			return false; // Move A after B
		}
		if (B.Quantity == 0 && A.Quantity != 0) {
			return true; // Keep A before B
		}

		// If both quantities are non-zero or both are zero, apply the sorting logic
		switch (SortMode) {
		case EInventorySortMode::ByNameDescending:
			return A.ItemID.RowName.Compare(B.ItemID.RowName) > 0;

		case EInventorySortMode::ByNameAscending:
			return A.ItemID.RowName.Compare(B.ItemID.RowName) < 0;

		case EInventorySortMode::ByQuantityDescending:
			return A.Quantity > B.Quantity;

		case EInventorySortMode::ByQuantityAscending:
			return A.Quantity < B.Quantity;
		}
		return false;
		});

	UE_LOG(LogTemp, Log, TEXT("Inventory sorted successfully"));
}

/**
 * Gets the stack size for a given item.
 *
 * @param ExistingItem The item to get the stack size for.
 * @return The stack size of the item.
 */
int32 UMyInventory::getStackSize(FSlot* ExistingItem) const {
	static const FString ContextString(TEXT("RowHandle Context"));

	if (ItemsDataTable) {
		FItemRow* Row = ItemsDataTable->FindRow<FItemRow>(ExistingItem->ItemID.RowName, ContextString);
		if (Row)
			return Row->StackSize;
	}
	return -1;
}

/**
 * Resizes the inventory to the specified size.
 *
 * @param InventoryType The type of inventory to resize.
 */
void UMyInventory::ResizeInventory(EItemTypes InventoryType) {
	TArray<FSlot>* inventory = GetInventory(InventoryType);
	int32 inventorySize = GetInventorySize(InventoryType);

	UE_LOG(LogTemp, Warning, TEXT("Resizing to %d"), inventorySize);
	if (inventory) inventory->SetNum(inventorySize);
}

/**
 * Gets the inventory for a specified type.
 *
 * @param InventoryType The type of inventory to get.
 * @return A pointer to the inventory array.
 */
TArray<FSlot>* UMyInventory::GetInventory(EItemTypes InventoryType) {
	switch (InventoryType) {
	case EItemTypes::Food:
		return &AllItems.Food;
	}

	return NULL;
}

/**
 * Gets the size of the inventory for a specified type.
 *
 * @param InventoryType The type of inventory to get the size of.
 * @return The size of the inventory.
 */
int32 UMyInventory::GetInventorySize(EItemTypes InventoryType) const {
	switch (InventoryType) {
	case EItemTypes::Food:
		return AllItems.FoodInventorySize;
	}
	return 0;
}

/**
 * Sets the size of the inventory for a specified type.
 *
 * @param InventoryType The type of inventory to set the size of.
 * @param NewSize The new size of the inventory.
 * @return The new size of the inventory.
 */
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
