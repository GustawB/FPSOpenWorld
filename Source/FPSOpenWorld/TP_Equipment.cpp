// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Equipment.h"

// Sets default values for this component's properties
UTP_Equipment::UTP_Equipment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTP_Equipment::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTP_Equipment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AssaultRifleAmmo = AmmoTypes[0];
	ShotgunAmmo = AmmoTypes[1];
	GrenadeLauncherAmmo = AmmoTypes[2];
	PistolAmmo = AmmoTypes[3];
}

