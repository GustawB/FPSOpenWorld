// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_Equipment.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSOPENWORLD_API UTP_Equipment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTP_Equipment();

	//Owner's max health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body")
		float MaxHealth{ 100 };

	//Owner's current health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body")
		float CurrentHealth{ 100 };

	//Owner's stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body")
		int Stamina{ 100 };

	//Owner's Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int AssaultRifleAmmo{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int ShotgunAmmo{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int GrenadeLauncherAmmo{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int PistolAmmo{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int AmmoInMag{ 0 };

	//Array storing ammo for every weapon type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> AmmoTypes{ 30, 6, 6, 8 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentWeapon{ 4 };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
