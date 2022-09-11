// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "FPSOpenWorldCharacter.h"
#include "FPSOpenWorldProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

}

void UTP_WeaponComponent::PlayWeaponAddOns(bool bIsCharacterTheUser)
{
	// Try and play the sound if specified. FireSound is USoundBase
	if (FireSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, FireSound, WeaponMeshReference->GetComponentLocation(), WeaponMeshReference->GetComponentRotation(), 1.0f, 1.0f, 0.0f, nullptr, nullptr, true);
	}

	//Try and spawn the particle if specified
	if (MuzzleFlash != nullptr)
	{
		if (WeaponMeshReference != nullptr)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, WeaponMeshReference->GetSocketTransform("Muzzle"));
	}

	if (bIsCharacterTheUser)
		FireSequence = nullptr;
	else
		FireMontage = nullptr;

	// Try and play a firing animation if specified
	if (FireMontage != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
			AnimInstance->Montage_Play(FireMontage, 1.f);
	}
	else if(FireSequence != nullptr)
	{
		UAnimInstance* AnimInstance = EnemyMeshComponent->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			EnemyMeshComponent->PlayAnimation(FireSequence, false);
		}
	}

}


void UTP_WeaponComponent::Fire()
{
	if(Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (Character->GetEquipmentComponent()->AmmoInMag > 0)
	{
		/*// Try and fire a projectile
		if (ProjectileClass != nullptr)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
				const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// Spawn the projectile at the muzzle
				//World->SpawnActor<AFPSOpenWorldProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}*/

		PlayWeaponAddOns(true);
		--Character->GetEquipmentComponent()->AmmoInMag;

		//Trace-based shooting. Below I'm getting the center of the camera view and I'm drawing a line trace from there
		FHitResult TraceHitResult{};
		FVector DirectionVector{ Character->GetFirstPersonCameraComponent()->GetForwardVector() };
		//We are drawin a trace in front of the camera
		FVector TraceStartingPoint{ Character->GetFirstPersonCameraComponent()->GetComponentLocation() + DirectionVector * 150};
		FVector TraceEnd{ (DirectionVector * ShootingDistance) + TraceStartingPoint };
		FCollisionQueryParams CollisionParams;
		DrawDebugLine(GetWorld(), TraceStartingPoint, TraceEnd, FColor::Green, false, 1, 0, 1);
		if (GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStartingPoint, TraceEnd, ECC_Visibility, CollisionParams))
		{
			if (TraceHitResult.bBlockingHit)
			{
				AFPSOpenWorldEnemy* HitEnemy = Cast<AFPSOpenWorldEnemy>(TraceHitResult.GetActor());
				if (HitEnemy != nullptr)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("Enemy"));
					HitEnemy->EnemyHealth -= WeaponDamage;
					if (HitEnemy->EnemyHealth <= 0.0f)
						HitEnemy->Kill();
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red , TEXT("No enemy"));
				}
			}
		}
	}
}

void UTP_WeaponComponent::EnemyFire(AFPSOpenWorldEnemy* Enemy)
{
	PlayWeaponAddOns(false);

	FHitResult TraceHitResult{};
	FVector TraceStartingPoint{ WeaponMeshReference->GetSocketLocation("Muzzle") };
	//Right vector because weapon is rotated
	FVector DirectionVector{ WeaponMeshReference->GetRightVector() };
	//*(-1)because weapon is rotated left, not right
	FVector TraceEnd{ (DirectionVector * ShootingDistance) + TraceStartingPoint * (-1) };
	FCollisionQueryParams CollisionParams;
	DrawDebugLine(GetWorld(), TraceStartingPoint, TraceEnd, FColor::Green, false, 1, 0, 1);
	if (GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStartingPoint, TraceEnd, ECC_Visibility, CollisionParams))
	{
		if (TraceHitResult.bBlockingHit)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Enemy hit registered"));
			AFPSOpenWorldCharacter* HitCharacter = Cast<AFPSOpenWorldCharacter>(TraceHitResult.GetActor());
			if (HitCharacter != nullptr)
				HitCharacter->GetEquipmentComponent()->CurrentHealth -= WeaponDamage;
		}
		--Enemy->GetEquipmentComponent()->AmmoInMag;
	}

}

void UTP_WeaponComponent::EnemyReload(AFPSOpenWorldEnemy* Enemy)
{
	Enemy->bIsReloading = true;
	Enemy->GetEquipmentComponent()->AmmoInMag = MagSize;
	UAnimInstance* AnimInstance = EnemyMeshComponent->GetAnimInstance();
	if (AnimInstance != nullptr && ReloadSequence != nullptr)
	{
		EnemyMeshComponent->PlayAnimation(ReloadSequence, false);
	}
}

void UTP_WeaponComponent::Reload()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (weaponType != WeaponType::NUMBER_OF_WEAPON_TYPES)
	{
		int AmmoNeededForReload{ MagSize - Character->GetEquipmentComponent()->AmmoInMag };

		// Try and play the sound if specified
		if (ReloadSound != nullptr && AmmoNeededForReload > 0 && Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] > 0)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, Character->GetActorLocation());
		}

		//Reloading when we have enough ammo in equipment to fill one mag
		if (Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] >= AmmoNeededForReload)
		{
			Character->GetEquipmentComponent()->AmmoInMag += AmmoNeededForReload;
			Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] -= AmmoNeededForReload;
		}
		else //Reloading when we don't have enough ammo in equipment to fill one mag
		{
			Character->GetEquipmentComponent()->AmmoInMag += Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray];
			Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] = 0;
		}

	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TP_WeaponComponent.cpp line 121: Wrong Weapon Type"));
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(Character != nullptr)
	{
		// Unregister from the OnUseItem Event
		Character->OnUseItem.RemoveDynamic(this, &UTP_WeaponComponent::Fire);
	}
}

void UTP_WeaponComponent::AttachWeapon(AFPSOpenWorldCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character != nullptr)
	{
		//Determine weapon type
		IndexForWeaponArray = static_cast<int>(weaponType);

		// Attach the weapon to the First Person Character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		GetOwner()->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

		// Register so that Fire is called every time the character tries to use the item being held
		Character->OnUseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);

		// Register so that Fire is called every time the character tries to use the item being held
		Character->OnReloadWeapon.AddDynamic(this, &UTP_WeaponComponent::Reload);

		//Set ShootingSpeed of weapon
		Character->ShootingSpeed = RateOfFire;

		//Set current weapon in equipment
		Character->GetEquipmentComponent()->CurrentWeapon = IndexForWeaponArray;

		if (IndexForWeaponArray != static_cast<int>(WeaponType::NUMBER_OF_WEAPON_TYPES))
		{
			//Load ammo to mag when attaching weapon and we have enough ammo
			if (MagSize <= Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray])
			{
				Character->GetEquipmentComponent()->AmmoInMag = MagSize;
				Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] -= MagSize;
			}
			else //Load ammo to mag when attaching weapon and we don't have enough ammo
			{
				Character->GetEquipmentComponent()->AmmoInMag = Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray];
				Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] = 0;
			}
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TP_WeaponComponent.cpp line 172: Wrong Weapon Type"));
	}
}

void UTP_WeaponComponent::DetachWeapon(AFPSOpenWorldCharacter * TargetCharacter)
{
	Character = TargetCharacter;
	if (Character != nullptr)
	{
		FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
		GetOwner()->DetachFromActor(DetachmentRules);
		Character->OnUseItem.RemoveDynamic(this, &UTP_WeaponComponent::Fire);
		Character->OnReloadWeapon.RemoveDynamic(this, &UTP_WeaponComponent::Reload);
		Character->bIsUsingWeapon = false;

		//I'm getting two values of IndexForWeaponArray here, and I don't know why. Setting its default value to WeaponType::NUMBER_OF_WEAPON_TYPES
		//and then blocking that value by if statement below seems to fix the problem (or rather prevent; the problem stil;l exists but it doesn't impact the gameplay).
		if (IndexForWeaponArray != static_cast<int>(WeaponType::NUMBER_OF_WEAPON_TYPES))
		{
			//Unload ammo from mag when detaching weapon
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%lld"), IndexForWeaponArray));
			Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray] += Character->GetEquipmentComponent()->AmmoInMag;
			Character->GetEquipmentComponent()->AmmoInMag = 0;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%lld"), Character->GetEquipmentComponent()->AmmoTypes[IndexForWeaponArray]));
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TP_WeaponComponent.cpp line 194: Wrong Weapon Type"));
	}
}

