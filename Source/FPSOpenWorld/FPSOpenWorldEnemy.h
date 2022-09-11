// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "TP_Equipment.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/Character.h"
#include "FPSOpenWorldEnemy.generated.h"

UCLASS()
class FPSOPENWORLD_API AFPSOpenWorldEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSOpenWorldEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle DeathTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleDefaultsOnly)
		UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSourceComponent;

	UPROPERTY(VisibleDefaultsOnly)
	USkeletalMeshComponent* EnemyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimSequence* DeathSequence;

	UPROPERTY(VisibleDefaultsOnly)
		UTP_Equipment* TP_Equipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float DeathBodyExistTime{ 10.0f };

	UPROPERTY(EditAnywhere, Category = "Body")
		float EnemyHealth{ 100.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsAlive{ true };

	UPROPERTY(BlueprintReadWrite)
		bool bIsReloading{ false };

	UPROPERTY(BlueprintReadWrite)
		bool bIsShooting{ false };

	//ReloadDuration is equal to the length of the reload animation, you have to set it by yourself
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
		float ReloadDuration{ 2.0f };

	/** Returns TP_Equipment subobject. Its values will be modified, so the function isn't const **/
	UFUNCTION(BlueprintCallable)
		UTP_Equipment* GetEquipmentComponent() { return TP_Equipment; }

	void Kill();
};
