// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSOpenWorldEnemy.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMesh.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/ActorComponent.h"
#include "TP_WeaponComponent.generated.h"

class AFPSOpenWorldCharacter;

UENUM(BlueprintType)
enum class WeaponType : uint8
{
	AssaultRifle,
	Shotgun,
	SniperRifle,
	Pistol,

	NUMBER_OF_WEAPON_TYPES,
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSOPENWORLD_API UTP_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFPSOpenWorldProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* WeaponMeshReference {};

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** Particle to spawn each time we shoot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		WeaponType weaponType {};

	/** AnimMontage to play each time we fire, USE ONLY FOR THE PLAYER */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireMontage;

	/** AnimMontage to play each time we fire, USE ONLY FOR THE ENEMY */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimSequence* FireSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimSequence* ReloadSequence;

	/** Variable storing weapon's rate of fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float RateOfFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponDamage{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float ShootingDistance{ 10000.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int MagSize;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* EnemyMeshComponent;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AFPSOpenWorldCharacter* TargetCharacter);

	/** Detaches the actor from a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DetachWeapon(AFPSOpenWorldCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EnemyFire(AFPSOpenWorldEnemy* Enemy);

	/** Reloads a weapon */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void EnemyReload(AFPSOpenWorldEnemy* Enemy);

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//Responsible for playing animations, sounds and light related to the weapon
	void PlayWeaponAddOns(bool bIsCharacterTheUser);

	//Used to access array of ammunition inside equipment
	int IndexForWeaponArray{4};

private:
	/** The Character holding this weapon*/
	AFPSOpenWorldCharacter* Character;

};
