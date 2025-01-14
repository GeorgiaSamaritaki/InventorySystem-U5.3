// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemDataStructs.h"

#include "MyInventory.generated.h"

class UDataTable;

// Enum for Item Types 
UENUM(BlueprintType)
enum class EItemTypes : uint8 {
	None UMETA(DisplayName = "None"),
	Food UMETA(DisplayName = "Food"),
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
};

// Structure definition
USTRUCT(BlueprintType)
struct FSlot {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	FDataTableRowHandle ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	EItemTypes ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	int32 Quantity;

	FSlot()
		: ItemID()
		, ItemType(EItemTypes::None)
		, Quantity(0) {
	}

	FORCEINLINE bool IsEmpty() const { return Quantity == 0; }
	FORCEINLINE bool SameItem(FSlot other) const { return ItemID == other.ItemID; }
};

USTRUCT(BlueprintType)
struct FAllItems {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FSlot> Food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FSlot> Weapons;

	UPROPERTY()
	int32 FoodInventorySize = 8,
		WeaponInventorySize = 8;
};

USTRUCT(BlueprintType)
struct FItemRow : public FTableRowBase {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Name"))
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Description"))
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Thumbnail", MakeStructureDefaultValue = "None"))
	TObjectPtr<UTexture2D> Thumbnail;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "StackSize", MakeStructureDefaultValue = "0"))
	int32 StackSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Mesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<UStaticMesh> Mesh;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class INVENTORYSYSTEM_API UMyInventory : public UActorComponent {
	GENERATED_BODY()

public:
	UMyInventory();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventories")
	FAllItems AllItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventories")
	UDataTable* ItemsDataTable;

	UFUNCTION(BlueprintCallable)
	virtual bool AddToInventory(FSlot Item, int32 Index = 0);

	UFUNCTION(BlueprintCallable)
	virtual FSlot RemoveFromInventory(FSlot Item, int32 Index, int32 Quantity);

	UFUNCTION(BlueprintCallable)
	virtual void ResizeInventory(EItemTypes InventoryType);


private:
	TArray<FSlot>* GetInventory(EItemTypes InventoryType);
	int32 GetInventorySize(EItemTypes InventoryType) const;
	int32 SetInventorySize(EItemTypes InventoryType, int32 NewSize);
	int32 getStackSize(FSlot* ExistingItem) const;

	bool AddItemToIndex(TArray<FSlot>* Inventory, FSlot Item, int32 Index);
};
