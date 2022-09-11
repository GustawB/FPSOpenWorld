// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_AmmoComponent.generated.h"

UENUM(BlueprintType)
enum class AmmunitionType : uint8
{
	AssaultRifle,
	GrenadeLauncher,
	SniperRifle,
	Pistol,

	NUMBER_OF_AMMUNITION_TYPES,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSOPENWORLD_API UTP_AmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTP_AmmoComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Self-Defined Variables")
		int AmmoQuantity{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Self-Defined Variables")
		AmmunitionType ammunitionType;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
