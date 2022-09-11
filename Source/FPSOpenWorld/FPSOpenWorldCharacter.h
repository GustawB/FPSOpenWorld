// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TP_Equipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Character.h"
#include "FPSOpenWorldCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadWeapon);

UCLASS(config=Game)
class AFPSOpenWorldCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere)
	UCameraComponent* ADSCameraComponent;

	UPROPERTY(VisibleDefaultsOnly)
	UTP_Equipment* TP_Equipment;

public:
	AFPSOpenWorldCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bCanShowPickUpUMG{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bCanClosePickUpUMG{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bCanPickUpItem{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bCanThrowAwayItem{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bIsUsingWeapon{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bIsRunning{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bCanShoot{ true };	

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bIsShooting{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		bool bIsAiming{ false };

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
		float ShootingSpeed{ 1.0f };

	UPROPERTY(EditAnywhere, BLueprintReadWrite, Category = Gameplay)
		float FPSCameraFieldOfView{ 60.0f };

	// Used for ADS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* WeaponCameraReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* WeaponMeshReference;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;

	/** Delegate to whom weapons can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnReloadWeapon OnReloadWeapon;

protected:

	/** Fires a projectile. */
	void OnPrimaryAction();

	void Reload();

	/**Starts a timer for OnPrimaryAction*/
	void StartShooting();

	/**Stops a timer for OnPrimaryAction*/
	void StopShooting();

	FTimerHandle WeaponTimer{};

	FTimerHandle FireRateLimiter{};

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Called when player starts sprinting */
	void StartSprint();

	/** Called when player stops sprinting */
	void StopSprint();

	/** Called when player interacts with something */
	void StartInteraction();

	void StopInteraction();

	// Called when player discards his current weapon
	void StartThrowAway();

	void StopThrowAway();

	//Called to change aiming state
	void Aim();


	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	/** Responsible for rising players' speed during sprint */
	float MovementSpeed{ 0.6f };
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetFirstPersonCameraComponent() { return FirstPersonCameraComponent; }

	/** Returns TP_Equipment subobject. Its values will be modified, so the function isn't const **/
	UFUNCTION(BlueprintCallable)
	UTP_Equipment* GetEquipmentComponent() { return TP_Equipment; }

	//Used to allign ADS camera with current weapon's camera
	UFUNCTION(BlueprintCallable)
	void MoveADSCamera();

	//Activates main camera and deactivates ADS camera
	UFUNCTION(BlueprintCallable)
	void ActivateFPSCamera();

	//Activates ADS camera and seactivates main camera
	UFUNCTION(BlueprintCallable)
	void ActivateADSCamera();

};

