// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	//Enables Tick
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Setup the Sphere Collision
	SphereRadius = 32.f;

	//this->SetSimulatePhysics(true);
	//this->SetCollisionProfileName("WeaponComponent");
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereEndOverlap);
}

void UTP_PickUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AFPSOpenWorldCharacter* Character = Cast<AFPSOpenWorldCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Character != nullptr)
	{
		if (bCanBroadcastPickUp && Character->bCanPickUpItem)
		{
			//Notify actor that it's being picked up
			OnPickUp.Broadcast(Character);
		}

		if (Character->bCanThrowAwayItem)
		{
			OnThrowAway.Broadcast(Character);
			Character->bIsUsingWeapon = false;
		}
	}
}

void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AFPSOpenWorldCharacter* Character = Cast<AFPSOpenWorldCharacter>(OtherActor);
	if(Character != nullptr)
	{
		Character->bCanShowPickUpUMG = true;
		bCanBroadcastPickUp = true;
	}
}

void UTP_PickUpComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFPSOpenWorldCharacter* Character = Cast<AFPSOpenWorldCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->bCanClosePickUpUMG = true;
		bCanBroadcastPickUp = false;
	}
}

void UTP_PickUpComponent::DropAttachedWeapons(AFPSOpenWorldCharacter* Character)
{
	TArray<AActor*> Actors;
	Character->GetAttachedActors(Actors);
	for (auto& Actor : Actors)
	{
		//Checking if attached actor has PickUpComponent. If so, it will be dropped so the player can pick up new weapon
		UTP_PickUpComponent* PickUpComponent = Actor->FindComponentByClass<UTP_PickUpComponent>();
		if (PickUpComponent)
			PickUpComponent->OnThrowAway.Broadcast(Character);
	}
}
