// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "FPSOpenWorldEnemy.h"
#include "FPSOpenWorldCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TP_PickUpComponent.generated.h"

// Declaration of the delegate that will be called when someone picks this up
// The character picking this up is the parameter sent with the notification
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, AFPSOpenWorldCharacter*, PickUpCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThrowAway, AFPSOpenWorldCharacter*, PickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPSOPENWORLD_API UTP_PickUpComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
		void DropAttachedWeapons(AFPSOpenWorldCharacter* Character);

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPickUp OnPickUp;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnThrowAway OnThrowAway;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bCanBroadcastPickUp{ false };

	UTP_PickUpComponent();
protected:

	/** Called when the game starts */
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Code for when something overlaps this component */
	UFUNCTION()
		void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};