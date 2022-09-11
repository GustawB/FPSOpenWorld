// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSOpenWorldCharacter.h"
#include "FPSOpenWorldProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"


//////////////////////////////////////////////////////////////////////////
// AFPSOpenWorldCharacter

AFPSOpenWorldCharacter::AFPSOpenWorldCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	//FVector(-39.56f, 1.75f, 64.f)
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-49.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	ADSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ADSCAmeraComponent"));
	ADSCameraComponent->SetupAttachment(FirstPersonCameraComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//Create a TP_EquipmentComponent
	TP_Equipment = CreateDefaultSubobject<UTP_Equipment>(TEXT("TP_Equipment"));

	//Tick stuff
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

void AFPSOpenWorldCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSOpenWorldCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void AFPSOpenWorldCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFPSOpenWorldCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFPSOpenWorldCharacter::StopSprint);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSOpenWorldCharacter::StartInteraction);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AFPSOpenWorldCharacter::StopInteraction);
	PlayerInputComponent->BindAction("ThrowAway", IE_Pressed, this, &AFPSOpenWorldCharacter::StartThrowAway);
	PlayerInputComponent->BindAction("ThrowAway", IE_Released, this, &AFPSOpenWorldCharacter::StopThrowAway);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSOpenWorldCharacter::Reload);
	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &AFPSOpenWorldCharacter::Aim);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFPSOpenWorldCharacter::StartShooting);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &AFPSOpenWorldCharacter::StopShooting);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFPSOpenWorldCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFPSOpenWorldCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AFPSOpenWorldCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AFPSOpenWorldCharacter::LookUpAtRate);
}

void AFPSOpenWorldCharacter::OnPrimaryAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BUM"));
	// Trigger the OnItemUsed Event
	if (bCanShoot && bIsUsingWeapon)
	{
		GetWorldTimerManager().ClearTimer(FireRateLimiter);
		OnUseItem.Broadcast();//Shooting
		bCanShoot = false;
		FTimerDelegate TimerDelegate{};
		TimerDelegate.BindLambda([&]() {bCanShoot = true; });
		//if the delay for the timer was the same as the ShootingSpeed, bCanShoot would be still false, so the fire rate would be twice as big.
		//Now the difference is big enough so that bCanShoot will be true when using automatic weapons, and small enough so that no one will notice it.
		float FireRateLimiterDelay{ ShootingSpeed - 0.01f };
		GetWorldTimerManager().SetTimer(FireRateLimiter, TimerDelegate, ShootingSpeed, false, FireRateLimiterDelay);
	}
}

void AFPSOpenWorldCharacter::Reload()
{
	OnReloadWeapon.Broadcast();//Reloading
}

void AFPSOpenWorldCharacter::StartShooting()
{
	if (!bIsRunning)
	{
		bIsShooting = true;
		GetWorldTimerManager().SetTimer(WeaponTimer, this, &AFPSOpenWorldCharacter::OnPrimaryAction, ShootingSpeed, true, 0.0f);
	}
}

void AFPSOpenWorldCharacter::StopShooting()
{
	bIsShooting = false;
	GetWorldTimerManager().ClearTimer(WeaponTimer);
}

void AFPSOpenWorldCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPSOpenWorldCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AFPSOpenWorldCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * MovementSpeed);
	}
}

void AFPSOpenWorldCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value * MovementSpeed);
	}
}

void AFPSOpenWorldCharacter::StartSprint()
{
	MovementSpeed = 1.0f;
	bIsRunning = true;

	if (bIsAiming)
		Aim();
}

void AFPSOpenWorldCharacter::StopSprint()
{
	MovementSpeed = 0.6f;
	bIsRunning = false;
}

void AFPSOpenWorldCharacter::StartInteraction()
{
	bCanPickUpItem = true;
}

void AFPSOpenWorldCharacter::StopInteraction()
{
	bCanPickUpItem = false;
}

void AFPSOpenWorldCharacter::StartThrowAway()
{
	bCanThrowAwayItem = true;
}

void AFPSOpenWorldCharacter::StopThrowAway()
{
	bCanThrowAwayItem = false;
}

void AFPSOpenWorldCharacter::MoveADSCamera()
{
	//Getting weapon's camera locaation and rotation
	FVector NewADSCameraLocation{ WeaponCameraReference->GetComponentLocation() };
	FRotator NewADSCameraRotation{ WeaponCameraReference->GetComponentRotation() };
	
	//Assigning Weapon's camera transform to the ADS camera
	ADSCameraComponent->SetWorldLocation(NewADSCameraLocation);
	ADSCameraComponent->SetWorldRotation(NewADSCameraRotation);
}

void AFPSOpenWorldCharacter::ActivateADSCamera()
{
	FirstPersonCameraComponent->Deactivate();
	ADSCameraComponent->Activate();
}

void AFPSOpenWorldCharacter::ActivateFPSCamera()
{
	WeaponCameraReference->Deactivate();
	FirstPersonCameraComponent->Activate();
}

void AFPSOpenWorldCharacter::Aim()
{
	//If the player is using any weapon, he can aim
	if (bIsUsingWeapon && !bIsRunning)
	{
		if (!bIsAiming)
		{
			MoveADSCamera();
			ActivateADSCamera();

			bIsAiming = true;
		}
		else
		{
			ActivateFPSCamera();

			bIsAiming = false;
		}
	}

}

void AFPSOpenWorldCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AFPSOpenWorldCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AFPSOpenWorldCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPSOpenWorldCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPSOpenWorldCharacter::EndTouch);

		return true;
	}
	
	return false;
}
