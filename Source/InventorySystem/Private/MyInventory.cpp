// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInventory.h"

// Sets default values for this component's properties
UMyInventory::UMyInventory() {
	PrimaryComponentTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("MyInventory  code working"));
}


// Called when the game starts
void UMyInventory::BeginPlay() {
	Super::BeginPlay();


}


// Called every frame
void UMyInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

