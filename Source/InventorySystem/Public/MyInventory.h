// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDataStructs.h"

#include "MyInventory.generated.h"

class UDataTable;
// Enum Sorting types 
UENUM(BlueprintType)
enum class EInventorySortMode : uint8 {
	ByQuantityAscending,
	ByQuantityDescending,
	ByNameAscending,
	ByNameDescending
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class INVENTORYSYSTEM_API UMyInventory : public UActorComponent {
	GENERATED_BODY()

public:
	UMyInventory();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventories")
	FAllItems AllItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventories")
	UDataTable* ItemsDataTable;

	UFUNCTION(BlueprintCallable)
	virtual bool AddToInventory(FSlot Item);

	UFUNCTION(BlueprintCallable)
	virtual bool AddToInventoryOnIndex(FSlot Item, int32 Index);

	UFUNCTION(BlueprintCallable)
	virtual bool SwapItems(EItemTypes InventoryType, int32 IndexA, int32 IndexB);

	UFUNCTION(BlueprintCallable)
	virtual FSlot RemoveFromInventory(FSlot Item, int32 Index, int32 Quantity);

	UFUNCTION(BlueprintCallable)
	void SortInventory(EItemTypes InventoryType, EInventorySortMode SortMode);

	UFUNCTION(BlueprintCallable)
	void ResizeInventory(EItemTypes InventoryType);


private:
	bool AddItemToIndex(TArray<FSlot>* Inventory, FSlot Item, int32 Index);

	TArray<FSlot>* GetInventory(EItemTypes InventoryType);
	int32 GetInventorySize(EItemTypes InventoryType) const;
	int32 SetInventorySize(EItemTypes InventoryType, int32 NewSize);
	int32 getStackSize(FSlot* ExistingItem) const;
};
